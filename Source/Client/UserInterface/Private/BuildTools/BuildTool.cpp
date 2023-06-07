#include "BuildTool.h"

namespace AM
{
namespace Client
{

BuildTool::BuildTool(EventDispatcher& inUiEventDispatcher)
: uiEventDispatcher{inUiEventDispatcher}
, camera{}
, mapTileExtent{}
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

} // End namespace Client
} // End namespace AM
