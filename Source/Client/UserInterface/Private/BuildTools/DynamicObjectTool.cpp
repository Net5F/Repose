#include "DynamicObjectTool.h"
#include "World.h"
#include "Network.h"
#include "WorldObjectLocator.h"
#include "DynamicObjectCreateRequest.h"
#include "EntityType.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AMAssert.h"

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
, selectedObjectName{""}
, selectedSpriteSet{nullptr}
, selectedSpriteIndex{0}
{
}

void DynamicObjectTool::setSelectedObject(const std::string& name,
                                          const Rotation& rotation,
                                          const ObjectSpriteSet& spriteSet)
{
    AM_ASSERT(spriteSet.sprites[rotation.direction] != nullptr,
              "Tried to set invalid sprite.");

    // Save the name and sprite set.
    selectedObjectName = name;
    selectedSpriteSet = &spriteSet;

    // Iterate the set and track which indices contain a sprite.
    validSpriteIndices.clear();
    for (std::size_t i = 0; i < spriteSet.sprites.size(); ++i) {
        if (spriteSet.sprites[i] != nullptr) {
            validSpriteIndices.emplace_back(i);

            // Save the selected rotation's index within validSpriteIndices.
            if (i == static_cast<std::size_t>(rotation.direction)) {
                selectedSpriteIndex = (validSpriteIndices.size() - 1);
            }
        }
    }
    AM_ASSERT(validSpriteIndices.size() > 0, "Set didn't contain any sprites.");
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

    // If this tool is active, the user left clicked, and we have a selected 
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedSpriteSet != nullptr)) {
        // Tell the sim to add the object.
        Rotation rotation{static_cast<Rotation::Direction>(
            validSpriteIndices[selectedSpriteIndex])};
        network.serializeAndSend(DynamicObjectCreateRequest{
            selectedObjectName, mouseWorldPosition, rotation,
            selectedSpriteSet->numericID});

        // To deter users from placing a million entities, we deselect after 
        // placement. This also makes it faster if the user's next goal is 
        // to select the object and modify it.
        clearCurrentSelection();
    }
    else if (isActive && (buttonType == AUI::MouseButtonType::Right)
             && (selectedSpriteSet != nullptr)) {
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
    // If this tool isn't active, do nothing.
    if (!isActive) {
        return;
    }

    // Select the next sprite within the set, accounting for negative values.
    selectedSpriteIndex
        = (selectedSpriteIndex + amountScrolled + validSpriteIndices.size())
          % validSpriteIndices.size();

    // Set the newly selected sprite as a phantom at the current location.
    phantomSprites.clear();
    PhantomSpriteInfo phantomInfo{};
    phantomInfo.position = mouseWorldPosition;
    phantomInfo.sprite
        = selectedSpriteSet->sprites[validSpriteIndices[selectedSpriteIndex]];
    phantomSprites.push_back(phantomInfo);
}

void DynamicObjectTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms and color mods.
    phantomSprites.clear();
    spriteColorMods.clear();

    // TODO: Phantom isn't keeping up with cursor
    // If this tool is active.
    if (isActive) {
        // If we don't have a selection, check if we're hovering an object.
        if (selectedSpriteSet == nullptr) {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit a dynamic object, highlight it.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                EntityType entityType{world.registry.get<EntityType>(*entity)};
                if (entityType == EntityType::DynamicObject) {
                    spriteColorMods.emplace_back(*entity, highlightColor);
                }
            }
        }
        else {
            // We have a selection. Set the selected sprite as a phantom at 
            // the new location.
            PhantomSpriteInfo phantomInfo{};
            phantomInfo.position = mouseWorldPosition;
            phantomInfo.sprite
                = selectedSpriteSet
                      ->sprites[validSpriteIndices[selectedSpriteIndex]];
            phantomSprites.push_back(phantomInfo);
        }
    }
}

void DynamicObjectTool::clearCurrentSelection()
{
    selectedObjectName = "";
    selectedSpriteSet = nullptr;
    validSpriteIndices.clear();
    selectedSpriteIndex = 0;
    phantomSprites.clear();
    spriteColorMods.clear();
    if (onSelectionCleared != nullptr) {
        onSelectionCleared();
    }
}

} // End namespace Client
} // End namespace AM
