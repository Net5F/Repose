#include "EntityTool.h"
#include "World.h"
#include "Network.h"
#include "WorldObjectLocator.h"
#include "EntityCreateRequest.h"
#include "IsClientEntity.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AMAssert.h"

namespace AM
{
namespace Client 
{

EntityTool::EntityTool(const World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, worldObjectLocator{inWorldObjectLocator}
, highlightColor{200, 200, 200, 255}
, selectedEntityName{""}
, selectedSpriteSet{nullptr}
, selectedSpriteIndex{0}
{
}

void EntityTool::setSelectedEntity(
    const std::string& name, const ObjectSpriteSet& spriteSet,
    Uint8 defaultSpriteIndex)
{
    AM_ASSERT(spriteSet.sprites[defaultSpriteIndex] != nullptr,
              "Tried to set invalid sprite.");

    // Save the name and sprite set.
    selectedEntityName = name;
    selectedSpriteSet = &spriteSet;

    // Iterate the set and track which indices contain a sprite.
    validSpriteIndices.clear();
    for (std::size_t i = 0; i < spriteSet.sprites.size(); ++i) {
        if (spriteSet.sprites[i] != nullptr) {
            validSpriteIndices.emplace_back(i);

            // Save the index of defaultSpriteIndex within validSpriteIndices.
            if (i == defaultSpriteIndex) {
                selectedSpriteIndex = (validSpriteIndices.size() - 1);
            }
        }
    }
    AM_ASSERT(validSpriteIndices.size() > 0, "Set didn't contain any sprites.");
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
    ignore(cursorPosition);

    // If this tool is active, the user left clicked, and we have a selected 
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedSpriteSet != nullptr)) {
        // Tell the sim to add the entity.
        const Sprite* sprite{selectedSpriteSet->sprites[static_cast<Uint8>(
            validSpriteIndices[selectedSpriteIndex])]};
        network.serializeAndSend(
            EntityCreateRequest{mouseWorldPosition, sprite->numericID});

        // To deter users from placing a million entities, we deselect after 
        // placement. This also makes it faster if the user's next goal is 
        // to select the entity and modify it.
        clearCurrentSelection();
    }
    else if (isActive && (buttonType == AUI::MouseButtonType::Right)
             && (selectedSpriteSet != nullptr)) {
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

// TODO: This highlight behavior is acting weird, fix it.
void EntityTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // TODO: Phantom isn't keeping up with cursor
    // If this tool is active.
    if (isActive) {
        // If we don't have a selection, check if we're hovering an object.
        if (selectedSpriteSet == nullptr) {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit an entity, check if it's a non-client entity.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                if (!(world.registry.all_of<IsClientEntity>(*entity))) {
                    // We hit a non-client entity. Highlight it.
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

void EntityTool::clearCurrentSelection()
{
    selectedEntityName = "";
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
