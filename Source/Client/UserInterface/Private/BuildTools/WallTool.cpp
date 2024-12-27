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

            network.serializeAndSend(TileAddLayer{
                phantomInfo.tilePosition, TileOffset{}, TileLayer::Type::Wall,
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
    if (isActive && selectedGraphicSet) {
        // Add the appropriate phantom walls.
        addPhantomWalls();
    }
}

void WallTool::addPhantomWalls()
{
    // Note: If the player is on e.g. level 0 and places a wall on a tile that 
    //       has a full-height terrain block on it, the wall will look like 
    //       it's on level 1. If the player then goes to level 1 and places 
    //       a wall next to it, it won't attach as expected (because it's 
    //       technically not on the same level).
    //       There are a handful of possible solutions, but we want to see that 
    //       it's truly a problem before doing anything about it.

    // Calculate the mouse's position, relative to the top left of the tile
    // that it's on.
    Vector3 tileOrigin{mouseTilePosition.getOriginPoint()};
    float relativeX{std::abs(mouseWorldPoint.x - tileOrigin.x)};
    float relativeY{std::abs(mouseWorldPoint.y - tileOrigin.y)};

    // If the mouse is closer to the top of the tile.
    if (relativeY < relativeX) {
        addNorthWallPhantom();
    }
    else {
        // The mouse is closer to the left side of the tile, or it's equal.
        addWestWallPhantom();
    }
}

void WallTool::addNorthWallPhantom()
{
    // If the tile has a West wall, add a NE gap fill.
    const Tile* tile{world.tileMap.cgetTile(mouseTilePosition)};
    if (tile && tile->findLayer(TileLayer::Type::Wall, Wall::Type::West)) {
        pushPhantomWall(mouseTilePosition, Wall::Type::NorthEastGapFill,
                        *selectedGraphicSet);
    }
    else {
        // No West wall, add the North wall.
        // Note: If there's a NorthWestGapFill, this will replace it.
        // Note: This also handles the case where the tile doesn't exist.
        pushPhantomWall(mouseTilePosition, Wall::Type::North,
                        *selectedGraphicSet);
    }

    // If there's a tile to the NE that we might've formed a corner with.
    TilePosition northeastPos{mouseTilePosition.x + 1, mouseTilePosition.y - 1,
                              mouseTilePosition.z};
    const Tile* northeastTile{world.tileMap.cgetTile(northeastPos)};
    if (!northeastTile) {
        return;
    }

    // If the NorthEast tile has a West wall.
    auto* northeastWestWall{
        northeastTile->findLayer(TileLayer::Type::Wall, Wall::Type::West)};
    if (!northeastWestWall) {
        return;
    }

    // We formed a corner. Check if the tile to the east has a wall.
    // Note: We know this tile is in the map bounds cause there's a NorthEast 
    //       tile, but the chunk may not exist yet.
    TilePosition eastPos{mouseTilePosition.x + 1, mouseTilePosition.y,
                         mouseTilePosition.z};
    const Tile* eastTile{world.tileMap.cgetTile(eastPos)};
    if (!eastTile || eastTile->getLayers(TileLayer::Type::Wall).size() == 0) {
        // The tile has no walls. Add a NorthWestGapFill.
        pushPhantomWall(eastPos, Wall::Type::NorthWestGapFill,
                        *selectedGraphicSet);
    }
    else if (auto* eastNorthWestGapFill{eastTile->findLayer(
                 TileLayer::Type::Wall, Wall::Type::NorthWestGapFill)}) {
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

void WallTool::addWestWallPhantom()
{
    // Add the West wall.
    // Note: If this tile already has a West wall or NW gap fill, this will
    //       replace it.
    pushPhantomWall(mouseTilePosition, Wall::Type::West, *selectedGraphicSet);

    // If the tile has a North wall, switch it to a NorthEast gap fill.
    if (const Tile * tile{world.tileMap.cgetTile(mouseTilePosition)}) {
        if (auto* northWall{
                tile->findLayer(TileLayer::Type::Wall, Wall::Type::North)}) {
            pushPhantomWall(mouseTilePosition, Wall::Type::NorthEastGapFill,
                            static_cast<const WallGraphicSet&>(
                                northWall->graphicSet.get()));
        }
    }

    // If there's a tile to the SW that we might've formed a corner with.
    TilePosition southwestPos{mouseTilePosition.x - 1, mouseTilePosition.y + 1,
                              mouseTilePosition.z};
    const Tile* southwestTile{world.tileMap.cgetTile(southwestPos)};
    if (!southwestTile) {
        return;
    }

    // If the SouthWest tile has a North wall or a NE gap fill.
    auto* southwestNorthWall{
        southwestTile->findLayer(TileLayer::Type::Wall, Wall::Type::North)};
    auto* southwestNorthEastGapFill{southwestTile->findLayer(
        TileLayer::Type::Wall, Wall::Type::NorthEastGapFill)};
    if (!southwestNorthWall && !southwestNorthEastGapFill) {
        return;
    }

    // We formed a corner. Check if the tile to the South has a wall.
    // Note: We know this tile is in the map bounds cause there's a SouthWest 
    //       tile, but the chunk may not exist yet.
    TilePosition southPos{mouseTilePosition.x, mouseTilePosition.y + 1,
                          mouseTilePosition.z};
    const Tile* southTile{world.tileMap.cgetTile(southPos)};
    if (!southTile || southTile->getLayers(TileLayer::Type::Wall).size() == 0) {
        // The tile has no walls. Add a NorthWestGapFill.
        pushPhantomWall(southPos, Wall::Type::NorthWestGapFill,
                        *selectedGraphicSet);
    }
    else if (auto* southNorthWestGapFill{
                 southTile->findLayer(TileLayer::Type::Wall,
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

void WallTool::pushPhantomWall(const TilePosition& tilePosition, Wall::Type wallType,
                               const WallGraphicSet& wallGraphicSet)
{
    phantomSprites.emplace_back(tilePosition, TileOffset{},
                                TileLayer::Type::Wall, wallType, Position{},
                                &wallGraphicSet, static_cast<Uint8>(wallType));
}

} // End namespace Client
} // End namespace AM
