#include "DynamicObjectTool.h"
#include "World.h"
#include "Network.h"
#include "WorldObjectLocator.h"
#include "DynamicObjectCreateRequest.h"
#include "EntityType.h"
#include "Name.h"
#include "Rotation.h"
#include "SpriteSets.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AMAssert.h"
#include "entt/entity/entity.hpp"

namespace AM
{
namespace Client 
{

DynamicObjectTool::DynamicObjectTool(const World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, worldObjectLocator{inWorldObjectLocator}
, highlightColor{255, 255, 255, 255}
, selectedObject{entt::null}
, selectedTemplateName{""}
, selectedTemplateSpriteSet{nullptr}
, selectedTemplateValidSpriteIndices{}
, selectedTemplateSpriteIndex{0}
{
}

void DynamicObjectTool::setSelectedTemplate(const std::string& name,
                                          const Rotation& rotation,
                                          const ObjectSpriteSet& spriteSet)
{
    AM_ASSERT(spriteSet.sprites[rotation.direction] != nullptr,
              "Tried to set invalid sprite.");

    // Save the name and sprite set.
    selectedTemplateName = name;
    selectedTemplateSpriteSet = &spriteSet;

    // Iterate the set and track which indices contain a sprite.
    selectedTemplateValidSpriteIndices.clear();
    for (std::size_t i = 0; i < spriteSet.sprites.size(); ++i) {
        if (spriteSet.sprites[i] != nullptr) {
            selectedTemplateValidSpriteIndices.emplace_back(
                static_cast<Uint8>(i));

            // Save the selected rotation's index within validSpriteIndices.
            if (i == static_cast<std::size_t>(rotation.direction)) {
                selectedTemplateSpriteIndex
                    = (selectedTemplateValidSpriteIndices.size() - 1);
            }
        }
    }
    AM_ASSERT(selectedTemplateValidSpriteIndices.size() > 0,
              "Set didn't contain any sprites.");
}

void DynamicObjectTool::setOnObjectSelected(
    std::function<void(entt::entity objectEntityID, const std::string& name,
                       const Rotation& rotation,
                       const ObjectSpriteSet& spriteSet)>
        inOnObjectSelected)
{
    onObjectSelected = inOnObjectSelected;
}

void DynamicObjectTool::setOnSelectionCleared(
    std::function<void(void)> inOnSelectionCleared)
{
    onSelectionCleared = inOnSelectionCleared;
}

void DynamicObjectTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().
    ignore(cursorPosition);

    // If this tool is active and the user left clicked.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
        // If a template is selected in the content panel.
        if (selectedTemplateSpriteSet != nullptr) {
            // Tell the sim to create an object based on the template.
            Rotation rotation{static_cast<Rotation::Direction>(
                selectedTemplateValidSpriteIndices
                    [selectedTemplateSpriteIndex])};
            network.serializeAndSend(DynamicObjectCreateRequest{
                selectedTemplateName, mouseWorldPosition, rotation,
                selectedTemplateSpriteSet->numericID});

            // To deter users from placing a million entities, we deselect after
            // placement. This also makes it faster if the user's next goal is
            // to select the object and modify it.
            clearCurrentSelection();
        }
        else {
            // We don't have a template selected. Check if we're clicking a new
            // object in the world.

            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit a dynamic object try to select it.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                trySelectObject(*entity);
            }
        }
    }
    else if (isActive && (buttonType == AUI::MouseButtonType::Right)) {
        // The user right clicked. Deselect the current selection.
        clearCurrentSelection();
    }
}

void DynamicObjectTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void DynamicObjectTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active or there isn't a template selected, do nothing.
    if (!isActive || (selectedTemplateSpriteSet == nullptr)) {
        return;
    }

    // Select the next sprite within the set, accounting for negative values.
    selectedTemplateSpriteIndex = (selectedTemplateSpriteIndex + amountScrolled
                                   + selectedTemplateValidSpriteIndices.size())
                                  % selectedTemplateValidSpriteIndices.size();

    // Set the newly selected sprite as a phantom at the current location.
    phantomSprites.clear();
    PhantomSpriteInfo phantomInfo{};
    phantomInfo.position = mouseWorldPosition;
    phantomInfo.sprite = getSelectedTemplateSprite();
    phantomSprites.push_back(phantomInfo);
}

void DynamicObjectTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms and color mods.
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected object, highlight it.
    if (selectedObject != entt::null) {
        spriteColorMods.emplace_back(selectedObject, highlightColor);
    }

    // If this tool is active.
    if (isActive) {
        // If we have a selection. Set the selected sprite as a phantom at 
        // the new location.
        if (selectedTemplateSpriteSet != nullptr) {
            PhantomSpriteInfo phantomInfo{};
            phantomInfo.position = mouseWorldPosition;
            phantomInfo.sprite = getSelectedTemplateSprite();
            phantomSprites.push_back(phantomInfo);
        }
        // Else we don't have a selection. Check if we're hovering an object.
        else {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit a dynamic object that isn't selected, highlight it.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                EntityType entityType{world.registry.get<EntityType>(*entity)};
                if ((entityType == EntityType::DynamicObject)
                    && (*entity != selectedObject)) {
                    spriteColorMods.emplace_back(*entity, highlightColor);
                }
            }
        }
    }
}

void DynamicObjectTool::onMouseLeave()
{
    isActive = false;
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected widget, keep it highlighted.
    if (selectedObject != entt::null) {
        spriteColorMods.emplace_back(selectedObject, highlightColor);
    }
}

void DynamicObjectTool::trySelectObject(entt::entity entity)
{
    // If the entity is a dynamic object and isn't already selected, select it.
    EntityType entityType{world.registry.get<EntityType>(entity)};
    if ((entityType == EntityType::DynamicObject)
        && (entity != selectedObject)) {
        clearCurrentSelection();

        const Name& name{world.registry.get<Name>(entity)};
        const Rotation& rotation{world.registry.get<Rotation>(entity)};
        const ObjectSpriteSet& spriteSet{
            world.registry.get<ObjectSpriteSet>(entity)};

        selectedObject = entity;

        if (onObjectSelected != nullptr) {
            onObjectSelected(selectedObject, name.name, rotation, spriteSet);
        }
    }
}

void DynamicObjectTool::clearCurrentSelection()
{
    // If we have an object or template selected, clear everything.
    if ((selectedObject != entt::null)
        || (selectedTemplateSpriteSet != nullptr)) {
        selectedObject = entt::null;
        selectedTemplateName = "";
        selectedTemplateSpriteSet = nullptr;
        selectedTemplateValidSpriteIndices.clear();
        selectedTemplateSpriteIndex = 0;
        phantomSprites.clear();
        spriteColorMods.clear();

        if (onSelectionCleared != nullptr) {
            onSelectionCleared();
        }
    }
}

const Sprite* DynamicObjectTool::getSelectedTemplateSprite()
{
    if (selectedTemplateSpriteSet == nullptr) {
        return nullptr;
    }

    return selectedTemplateSpriteSet->sprites
        [selectedTemplateValidSpriteIndices[selectedTemplateSpriteIndex]];
}

} // End namespace Client
} // End namespace AM
