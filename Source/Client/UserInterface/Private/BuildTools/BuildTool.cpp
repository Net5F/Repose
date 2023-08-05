#include "BuildTool.h"
#include "World.h"
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
, mouseWorldPosition{}
, mouseTilePosition{}
, isActive{false}
, phantomSprites{}
, spriteColorMods{}
{
}

std::span<const PhantomSpriteInfo> BuildTool::getPhantomSprites() const
{
    return phantomSprites;
}

std::span<const SpriteColorModInfo> BuildTool::getSpriteColorMods() const
{
    return spriteColorMods;
}

void BuildTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    ignore(inSelectedSpriteSet);
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
    SDL_FPoint screenPoint{static_cast<float>(cursorPosition.x),
                           static_cast<float>(cursorPosition.y)};
    Position newWorldPosition{Transforms::screenToWorld(screenPoint, camera)};
    TilePosition newTilePosition{newWorldPosition.asTilePosition()};

    // If the mouse is within the world bounds, save the new tile position.
    if (mapTileExtent.containsPosition(newTilePosition)) {
        mouseWorldPosition = newWorldPosition;
        mouseTilePosition = newTilePosition;
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
    phantomSprites.clear();
    spriteColorMods.clear();
}

} // End namespace Client
} // End namespace AM
