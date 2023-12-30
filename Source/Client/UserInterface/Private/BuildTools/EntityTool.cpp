#include "EntityTool.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "WorldObjectLocator.h"
#include "EntityInitRequest.h"
#include "IsClientEntity.h"
#include "AnimationState.h"
#include "QueuedEvents.h"
#include "AMAssert.h"
#include "entt/entity/entity.hpp"

namespace AM
{
namespace Client
{

EntityTool::EntityTool(World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       Network& inNetwork, SpriteData& inSpriteData)
: BuildTool(inWorld, inNetwork)
, worldObjectLocator{inWorldObjectLocator}
, spriteData{inSpriteData}
, highlightColor{255, 255, 255, 255}
, selectedEntityID{entt::null}
, selectedTemplateName{""}
, selectedTemplateAnimationState{}
{
    // Listen for destruction events for the current selected object.
    world.registry.on_destroy<entt::entity>()
        .connect<&EntityTool::onEntityDestroyed>(this);
}

void EntityTool::setSelectedTemplate(const Name& name,
                                     const AnimationState& animationState)
{
    // Save the name and animation state.
    selectedTemplateName = name;
    selectedTemplateAnimationState = animationState;
}

void EntityTool::setOnEntitySelected(
    std::function<void(entt::entity entity)> inOnEntitySelected)
{
    onEntitySelected = inOnEntitySelected;
}

void EntityTool::setOnSelectionCleared(
    std::function<void(void)> inOnSelectionCleared)
{
    onSelectionCleared = inOnSelectionCleared;
}

void EntityTool::onMouseDown(AUI::MouseButtonType buttonType,
                             const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active and the user left clicked.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
        // If a template is selected in the content panel.
        if (selectedTemplateAnimationState.spriteSetType
            != SpriteSet::Type::None) {
            // Tell the sim to create an object based on the template.
            network.serializeAndSend(EntityInitRequest{
                entt::null, selectedTemplateName, mouseWorldPosition,
                selectedTemplateAnimationState});

            // To deter users from placing a million entities, we deselect after
            // placement. This also makes it faster if the user's next goal is
            // to select the object and modify it.
            clearCurrentSelection();
        }
        // Else we don't have a template selected. If we hit an entity, try
        // to select it.
        else {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                trySelectEntity(*entity);
            }
        }
    }
    else if (isActive && (buttonType == AUI::MouseButtonType::Right)) {
        // The user right clicked. Deselect the current selection.
        clearCurrentSelection();
    }
}

void EntityTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void EntityTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active or there isn't a template selected, do nothing.
    if (!isActive
        || (selectedTemplateAnimationState.spriteSetType
            == SpriteSet::Type::None)) {
        return;
    }

    const ObjectSpriteSet& spriteSet{spriteData.getObjectSpriteSet(
        selectedTemplateAnimationState.spriteSetID)};
    const auto& sprites{spriteSet.sprites};

    // Scroll the desired amount of times, skipping empty slots.
    int stepsRemaining{std::abs(amountScrolled)};
    int stepSize{amountScrolled / std::abs(amountScrolled)};
    while (stepsRemaining > 0) {
        // Find the next non-empty index, accounting for negative values.
        int nextIndex{selectedTemplateAnimationState.spriteIndex};
        nextIndex = static_cast<int>(nextIndex + stepSize + sprites.size())
                    % sprites.size();

        while (sprites[nextIndex] == nullptr) {
            nextIndex = static_cast<int>(nextIndex + stepSize + sprites.size())
                        % sprites.size();
        }

        selectedTemplateAnimationState.spriteIndex = nextIndex;
        stepsRemaining--;
    }

    // Set the newly selected sprite as a phantom at the current location.
    phantomSprites.clear();
    PhantomSpriteInfo phantomInfo{};
    phantomInfo.position = mouseWorldPosition;
    phantomInfo.sprite = getSelectedTemplateSprite();
    phantomSprites.push_back(phantomInfo);
}

void EntityTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms and color mods.
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected entity, highlight it.
    if (selectedEntityID != entt::null) {
        spriteColorMods.emplace_back(selectedEntityID, highlightColor);
    }

    // If this tool is active.
    if (isActive) {
        // If we have a template selected, set the selected sprite as a phantom
        // at the new location.
        if (selectedTemplateAnimationState.spriteSetType
            != SpriteSet::Type::None) {
            PhantomSpriteInfo phantomInfo{};
            phantomInfo.position = mouseWorldPosition;
            phantomInfo.sprite = getSelectedTemplateSprite();
            phantomSprites.push_back(phantomInfo);
        }
        // Else we don't have a selection. Check if we're hovering an entity.
        else {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit a non-client entity that isn't selected, highlight it.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                if (!(world.registry.all_of<IsClientEntity>(*entity))
                    && (*entity != selectedEntityID)) {
                    spriteColorMods.emplace_back(*entity, highlightColor);
                }
            }
        }
    }
}

void EntityTool::onMouseLeave()
{
    isActive = false;
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected entity, keep it highlighted.
    if (selectedEntityID != entt::null) {
        spriteColorMods.emplace_back(selectedEntityID, highlightColor);
    }
}

void EntityTool::trySelectEntity(entt::entity entity)
{
    // If this isn't a client entity and it isn't already selected, select it.
    if (!(world.registry.all_of<IsClientEntity>(entity))
        && (entity != selectedEntityID)) {
        clearCurrentSelection();

        selectedEntityID = entity;

        if (onEntitySelected != nullptr) {
            onEntitySelected(selectedEntityID);
        }
    }
}

void EntityTool::onEntityDestroyed(entt::registry&, entt::entity entity)
{
    // If the selected entity was destroyed, clear our selection.
    if (entity == selectedEntityID) {
        clearCurrentSelection();
    }
}

void EntityTool::clearCurrentSelection()
{
    // If we have an entity or template selected, clear everything.
    if ((selectedEntityID != entt::null)
        || (selectedTemplateAnimationState.spriteSetType
            != SpriteSet::Type::None)) {
        selectedEntityID = entt::null;
        selectedTemplateName = {};
        selectedTemplateAnimationState = {};
        phantomSprites.clear();
        spriteColorMods.clear();

        if (onSelectionCleared != nullptr) {
            onSelectionCleared();
        }
    }
}

const Sprite* EntityTool::getSelectedTemplateSprite()
{
    if (selectedTemplateAnimationState.spriteSetType == SpriteSet::Type::None) {
        return nullptr;
    }

    const ObjectSpriteSet& spriteSet{spriteData.getObjectSpriteSet(
        selectedTemplateAnimationState.spriteSetID)};
    return spriteSet.sprites[selectedTemplateAnimationState.spriteIndex];
}

} // End namespace Client
} // End namespace AM
