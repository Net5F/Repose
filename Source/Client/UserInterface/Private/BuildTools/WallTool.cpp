#include "WallTool.h"
#include "World.h"
#include "Network.h"
#include "TileAddLayer.h"
#include "Transforms.h"
#include "QueuedEvents.h"
#include <cmath>

namespace AM
{
namespace Client
{

WallTool::WallTool(World& inWorld, Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, selectedGraphicSet{nullptr}
{
}

void WallTool::setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet)
{
    // Note: This cast should be safe, since only floor covering graphic sets
    //       should be clickable while this tool is alive.
    selectedGraphicSet = static_cast<const WallGraphicSet*>(&inSelectedGraphicSet);
}

void WallTool::onMouseDown(AUI::MouseButtonType buttonType, const SDL_Point&)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active, the user left clicked, and we have a selected
    // graphic.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedGraphicSet != nullptr)) {
        // Iterate the phantom tiles and tell the sim to add them for real.
        for (const auto& phantomInfo : phantomSprites) {
            // Skip NorthWest gap fills since the tile map will auto-add them.
            if ((phantomInfo.wallType == Wall::Type::NorthWestGapFill)) {
                continue;
            }

            // We don't want to push NE fills when adding West walls (the map
            // will auto-add them). But we do want to push them when adding
            // North walls (to tiles with a West wall).
            Wall::Type wallType{phantomInfo.wallType};
            if (phantomInfo.wallType == Wall::Type::NorthEastGapFill) {
                // Check if there's a phantom West wall.
                auto it = std::find_if(
                    phantomSprites.begin(), phantomSprites.end(),
                    [](const auto& phantomInfo) {
                        return phantomInfo.wallType == Wall::Type::West;
                    });
                if (it != phantomSprites.end()) {
                    // Found a West wall, skip this NorthEast phantom.
                    continue;
                }
                else {
                    // No West wall, push a North (the map will handle turnin
                    // it into a NorthEast fill).
                    wallType = Wall::Type::North;
                }
            }

            network.serializeAndSend(
                TileAddLayer{phantomInfo.tilePosition, TileLayer::Type::Wall,
                             selectedGraphicSet->numericID, wallType});
        }
    }
}

void WallTool::onMouseUp(AUI::MouseButtonType, const SDL_Point&)
{
    // TODO: Add support for click-and-drag to set an extent instead of a
    //       single tile.
}

void WallTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void WallTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // If this tool is active and we have a selected graphic.
    if (isActive && (selectedGraphicSet != nullptr)) {
        // Add the appropriate phantom walls.
        addPhantomWalls(cursorPosition);
    }
}

void WallTool::addPhantomWalls(const SDL_Point& cursorPosition)
{
    // Calculate the mouse's position, relative to the top left of the tile
    // that it's on.
    SDL_FPoint screenPoint{static_cast<float>(cursorPosition.x),
                           static_cast<float>(cursorPosition.y)};
    TilePosition tilePosition{
        Transforms::screenToWorldTile(screenPoint, camera)};
    Position tileOrigin{tilePosition.getOriginPosition()};
    Position worldPosition{
        Transforms::screenToWorldRay(screenPoint, camera).origin};

    float relativeX{std::abs(worldPosition.x - tileOrigin.x)};
    float relativeY{std::abs(worldPosition.y - tileOrigin.y)};

    // If the mouse is closer to the top of the tile.
    if (relativeY < relativeX) {
        addNorthWallPhantom(tilePosition);
    }
    else {
        // The mouse is closer to the left side of the tile, or it's equal.
        addWestWallPhantom(tilePosition);
    }
}

void WallTool::addNorthWallPhantom(const TilePosition& tilePosition)
{
    const Tile* tile{world.tileMap.cgetTile(tilePosition)};
    if (!tile) {
        // These coords are likely outside the map bounds.
        return;
    }

    // If the tile has a West wall, add a NE gap fill.
    if (tile->findLayer(TileLayer::Type::Wall, Wall::Type::West)) {
        pushPhantomWall(tilePosition, Wall::Type::NorthEastGapFill,
                        *selectedGraphicSet);
    }
    else {
        // No West wall, add the North wall.
        // Note: If there's a NorthWestGapFill, this will replace it.
        pushPhantomWall(tilePosition, Wall::Type::North, *selectedGraphicSet);
    }

    // If there's a tile to the NE that we might've formed a corner with.
    TilePosition northeastPos{tilePosition.x + 1, tilePosition.y - 1,
                              tilePosition.z};
    if (world.tileMap.getTileExtent().containsPosition(northeastPos)) {
        const Tile& northeastTile{*(world.tileMap.cgetTile(northeastPos))};

        // If the NorthEast tile has a West wall.
        if (auto* northeastWestWall{northeastTile.findLayer(
                TileLayer::Type::Wall, Wall::Type::West)}) {
            // We formed a corner. Check if the tile to the east has a wall.
            // Note: We know this tile is valid cause there's a NorthEast tile.
            TilePosition eastPos{tilePosition.x + 1, tilePosition.y,
                                 tilePosition.z};
            const Tile& eastTile{*(world.tileMap.cgetTile(eastPos))};
            if (eastTile.getLayers(TileLayer::Type::Wall).size() == 0) {
                // The tile has no walls. Add a NorthWestGapFill.
                pushPhantomWall(eastPos, Wall::Type::NorthWestGapFill,
                                *selectedGraphicSet);
            }
            else if (auto* eastNorthWestGapFill{
                         eastTile.findLayer(TileLayer::Type::Wall,
                                            Wall::Type::NorthWestGapFill)}) {
                // The East tile has a NW gap fill. If its graphic set no longer
                // matches either surrounding wall, make it match the new wall.
                int gapFillID{eastNorthWestGapFill->graphicSet.get().numericID};
                int newNorthID{selectedGraphicSet->numericID};
                int westID{northeastWestWall->graphicSet.get().numericID};
                if ((gapFillID != newNorthID) && (gapFillID != westID)) {
                    pushPhantomWall(eastPos, Wall::Type::NorthWestGapFill,
                                    *selectedGraphicSet);
                }
            }
        }
    }
}

void WallTool::addWestWallPhantom(const TilePosition& tilePosition)
{
    const Tile* tile{world.tileMap.cgetTile(tilePosition)};
    if (!tile) {
        // These coords are likely outside the map bounds.
        return;
    }

    // Add the West wall.
    // Note: If this tile already has a West wall or NW gap fill, this will
    //       replace it.
    pushPhantomWall(tilePosition, Wall::Type::West, *selectedGraphicSet);

    // If the tile has a North wall, switch it to a NorthEast gap fill.
    if (auto* northWall{
            tile->findLayer(TileLayer::Type::Wall, Wall::Type::North)}) {
        pushPhantomWall(
            tilePosition, Wall::Type::NorthEastGapFill,
            static_cast<const WallGraphicSet&>(northWall->graphicSet.get()));
    }

    // If there's a tile to the SW that we might've formed a corner with.
    TilePosition southwestPos{tilePosition.x - 1, tilePosition.y + 1,
                              tilePosition.z};
    if (world.tileMap.getTileExtent().containsPosition(southwestPos)) {
        const Tile& southwestTile{*(world.tileMap.cgetTile(southwestPos))};

        // If the SouthWest tile has a North wall or a NE gap fill.
        auto* southwestNorthWall{
            southwestTile.findLayer(TileLayer::Type::Wall, Wall::Type::North)};
        auto* southwestNorthEastGapFill{southwestTile.findLayer(
            TileLayer::Type::Wall, Wall::Type::NorthEastGapFill)};
        if (southwestNorthWall || southwestNorthEastGapFill) {
            // We formed a corner. Check if the tile to the South has a wall.
            // Note: We know this tile is valid cause there's a SouthWest tile.
            TilePosition southPos{tilePosition.x, tilePosition.y + 1,
                                  tilePosition.z};
            const Tile& southTile{*(world.tileMap.cgetTile(southPos))};
            if (southTile.getLayers(TileLayer::Type::Wall).size() == 0) {
                // The tile has no walls. Add a NorthWestGapFill.
                pushPhantomWall(southPos, Wall::Type::NorthWestGapFill,
                                *selectedGraphicSet);
            }
            else if (auto* southNorthWestGapFill{
                         southTile.findLayer(TileLayer::Type::Wall,
                                             Wall::Type::NorthWestGapFill)}) {
                // The South tile has a NW gap fill. If its graphic set no
                // longer matches either surrounding wall, make it match the new
                // wall.
                int gapFillID{
                    southNorthWestGapFill->graphicSet.get().numericID};
                int newWestID{selectedGraphicSet->numericID};
                int northID{southwestNorthWall
                                ? southwestNorthWall->graphicSet.get().numericID
                                : southwestNorthEastGapFill->graphicSet.get()
                                      .numericID};
                if ((gapFillID != newWestID) && (gapFillID != northID)) {
                    pushPhantomWall(southPos, Wall::Type::NorthWestGapFill,
                                    *selectedGraphicSet);
                }
            }
        }
    }
}

void WallTool::pushPhantomWall(const TilePosition& tilePosition, Wall::Type wallType,
                               const WallGraphicSet& wallGraphicSet)
{
    phantomSprites.emplace_back(
        tilePosition, TileLayer::Type::Wall, wallType, Terrain::Height::Flat,
        Position{}, &(wallGraphicSet.graphics[wallType].getFirstSprite()));
}

} // End namespace Client
} // End namespace AM
