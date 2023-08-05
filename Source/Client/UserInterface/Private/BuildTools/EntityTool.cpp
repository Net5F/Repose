#include "EntityTool.h"
#include "WorldObjectLocator.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AMAssert.h"

namespace AM
{
namespace Client 
{

EntityTool::EntityTool(const World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       EventDispatcher& inUiEventDispatcher)
: BuildTool(inWorld, inUiEventDispatcher)
, worldObjectLocator{inWorldObjectLocator}
, selectedObjectName{""}
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
    selectedObjectName = name;
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

void EntityTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().
    ignore(cursorPosition);

    // If this tool is active, the user left clicked, and we have a selected 
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedSpriteSet != nullptr)) {
        // Tell the sim to add the layer.
        uiEventDispatcher.emplace<TileAddLayer>(
            mouseTilePosition.x, mouseTilePosition.y,
            TileLayer::Type::Object, selectedSpriteSet->numericID,
            static_cast<Uint8>(validSpriteIndices[selectedSpriteIndex]));
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
    phantomSprites.emplace_back(
        mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Object,
        Wall::Type::None, Position{},
        selectedSpriteSet->sprites[validSpriteIndices[selectedSpriteIndex]]);
}

void EntityTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // If this tool is active.
    if (isActive) {
        // If we don't have a selection, check if we're hovering an object.
        if (selectedSpriteSet == nullptr) {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit an entity, check if it's a dynamic object.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                // TODO: On this side, it won't have an OnUseScript. Need to 
                //       check its SupportedInteractions for Use
                //if (world.registry.all_of<OnUseScript>(*entity)) {
                //}
            }
        }
        else {
            // We have a selection. Set the selected sprite as a phantom at 
            // the new location.
            phantomSprites.emplace_back(
                0, 0, TileLayer::Type::None, Wall::Type::None,
                mouseWorldPosition,
                selectedSpriteSet
                    ->sprites[validSpriteIndices[selectedSpriteIndex]]);
        }
    }
}

} // End namespace Client
} // End namespace AM
