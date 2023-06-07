#include "FloorTool.h"
#include "ScreenPoint.h"
#include "Transforms.h"

namespace AM
{
namespace Client 
{
FloorTool::FloorTool(EventDispatcher& inUiEventDispatcher)
: BuildTool(inUiEventDispatcher)
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
}

void FloorTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
}

void FloorTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void FloorTool::onMouseWheel(int amountScrolled)
{
}

// TODO: Commit
void FloorTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Get the tile coordinate that the mouse is hovering over.
    ScreenPoint screenPoint{static_cast<float>(cursorPosition.x),
                            static_cast<float>(cursorPosition.y)};
    TilePosition tilePosition{Transforms::screenToTile(screenPoint, camera)};

    // If we have a selected sprite and the mouse is inside the world bounds.
    if ((selectedSpriteSet != nullptr)
        && mapTileExtent.containsPosition(tilePosition)) {
        // Set the selected sprite as a phantom at the new location.
        phantomTileSprites.clear();
        phantomTileSprites.emplace_back(
            tilePosition.x, tilePosition.y, TileLayer::Type::Floor,
            Wall::Type::None, &(selectedSpriteSet->sprite));
    }
}

void FloorTool::onMouseLeave()
{
}

} // End namespace Client
} // End namespace AM
