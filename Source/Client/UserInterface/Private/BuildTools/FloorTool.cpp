#include "FloorTool.h"
#include "World.h"
#include "Network.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Client
{
FloorTool::FloorTool(World& inWorld, Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, selectedGraphicSet{nullptr}
{
}

void FloorTool::setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet)
{
    // Note: This cast should be safe, since only floor graphic sets should be
    //       clickable while this tool is alive.
    selectedGraphicSet
        = static_cast<const FloorGraphicSet*>(&inSelectedGraphicSet);
}

void FloorTool::onMouseDown(AUI::MouseButtonType buttonType, const SDL_Point&)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active, the user left clicked, and we have a selected
    // graphic.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedGraphicSet != nullptr)) {
        // Tell the server to add the layer.
        network.serializeAndSend(TileAddLayer{
            mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Floor,
            selectedGraphicSet->numericID, 0});
    }
}

void FloorTool::onMouseUp(AUI::MouseButtonType, const SDL_Point&)
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
    phantomSprites.clear();

    // If this tool is active and we have a selected graphic.
    if (isActive && (selectedGraphicSet != nullptr)) {
        // Set the selected graphic as a phantom at the new location.
        phantomSprites.emplace_back(
            mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Floor,
            Wall::Type::None, Position{},
            &(selectedGraphicSet->graphic.getFirstSprite()));
    }
}

} // End namespace Client
} // End namespace AM
