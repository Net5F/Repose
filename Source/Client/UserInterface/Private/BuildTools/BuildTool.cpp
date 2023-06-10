#include "BuildTool.h"
#include "World.h"
#include "ScreenPoint.h"
#include "Transforms.h"
#include "Ignore.h"

namespace AM
{
namespace Client
{

BuildTool::BuildTool(const World& inWorld, EventDispatcher& inUiEventDispatcher)
: world{inWorld}
, uiEventDispatcher{inUiEventDispatcher}
, camera{}
, mapTileExtent{}
, mouseTilePosition{}
, isActive{false}
, phantomTileSprites{}
, tileSpriteColorMods{}
{
}

std::span<const PhantomTileSpriteInfo> BuildTool::getPhantomTileSprites() const
{
    return phantomTileSprites;
}

std::span<const TileSpriteColorModInfo> BuildTool::getTileSpriteColorMods() const
{
    return tileSpriteColorMods;
}

void BuildTool::setCamera(const Camera& inCamera)
{
    camera = inCamera;
}

void BuildTool::setTileMapExtent(const TileExtent& inTileExtent)
{
    mapTileExtent = inTileExtent;
}

void BuildTool::onMouseDown(AUI::MouseButtonType buttonType,
    const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
}

void BuildTool::onMouseUp(AUI::MouseButtonType buttonType,
                       const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
}

void BuildTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
}

void BuildTool::onMouseWheel(int amountScrolled)
{
    ignore(amountScrolled);
}

void BuildTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Get the tile coordinate that the mouse is hovering over.
    ScreenPoint screenPoint{static_cast<float>(cursorPosition.x),
                            static_cast<float>(cursorPosition.y)};
    TilePosition newPosition{Transforms::screenToTile(screenPoint, camera)};

    // If the mouse is within the world bounds, save the new tile position.
    if (mapTileExtent.containsPosition(newPosition)) {
        mouseTilePosition = newPosition;
        isActive = true;
    }
    else {
        // The mouse is outside the world bounds. Deactivate the tool.
        isActive = false;
    }
}

void BuildTool::onMouseLeave()
{
    isActive = false;
    phantomTileSprites.clear();
}

} // End namespace Client
} // End namespace AM
