#include "FloorTool.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "Ignore.h"

namespace AM
{
namespace Client 
{
FloorTool::FloorTool(const World& inWorld, EventDispatcher& inUiEventDispatcher)
: BuildTool(inWorld, inUiEventDispatcher)
, selectedSpriteSet{nullptr}
{
}

void FloorTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only floor sprite sets should be 
    //       clickable while this tool is alive.
    selectedSpriteSet = static_cast<const FloorSpriteSet*>(&inSelectedSpriteSet);
}

void FloorTool::onMouseDown(AUI::MouseButtonType buttonType,
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
            mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Floor,
            selectedSpriteSet->numericID, 0);
    }
}

void FloorTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
    // TODO: Add support for click-and-drag to set an extent instead of a 
    //       single tile.
}

void FloorTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void FloorTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomTileSprites.clear();

    // If this tool is active and we have a selected sprite.
    if (isActive && (selectedSpriteSet != nullptr)) {
        // Set the selected sprite as a phantom at the new location.
        phantomTileSprites.emplace_back(
            mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Floor,
            Wall::Type::None, &(selectedSpriteSet->sprite));
    }
}

} // End namespace Client
} // End namespace AM