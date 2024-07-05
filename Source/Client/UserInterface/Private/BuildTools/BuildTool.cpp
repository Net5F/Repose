#include "BuildTool.h"
#include "World.h"
#include "Network.h"
#include "Transforms.h"

namespace AM
{
namespace Client
{

BuildTool::BuildTool(World& inWorld, Network& inNetwork)
: world{inWorld}
, network{inNetwork}
, camera{}
, mapTileExtent{}
, mouseWorldPoint{}
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

void BuildTool::setSelectedGraphicSet(const GraphicSet&) {}

void BuildTool::setCamera(const Camera& inCamera)
{
    camera = inCamera;
}

void BuildTool::setTileMapExtent(const TileExtent& inTileExtent)
{
    mapTileExtent = inTileExtent;
}

void BuildTool::onMouseDown(AUI::MouseButtonType, const SDL_Point&) {}

void BuildTool::onMouseUp(AUI::MouseButtonType, const SDL_Point&) {}

void BuildTool::onMouseDoubleClick(AUI::MouseButtonType, const SDL_Point&) {}

void BuildTool::onMouseWheel(int) {}

void BuildTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Get the tile coordinate that the mouse is hovering over.
    SDL_FPoint screenPoint{static_cast<float>(cursorPosition.x),
                           static_cast<float>(cursorPosition.y)};
    Vector3 newWorldPoint{
        Transforms::screenToWorldTarget(screenPoint, camera)};
    TilePosition newTilePosition{
        Transforms::screenToWorldTile(screenPoint, camera)};

    // If the mouse is within the world bounds, save the new positions.
    if (mapTileExtent.containsPosition(newTilePosition)) {
        mouseWorldPoint = newWorldPoint;
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
