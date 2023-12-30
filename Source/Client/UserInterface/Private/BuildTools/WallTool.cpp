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
, selectedSpriteSet{nullptr}
{
}

void WallTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only floor covering sprite sets
    //       should be clickable while this tool is alive.
    selectedSpriteSet = static_cast<const WallSpriteSet*>(&inSelectedSpriteSet);
}

void WallTool::onMouseDown(AUI::MouseButtonType buttonType, const SDL_Point&)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active, the user left clicked, and we have a selected
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedSpriteSet != nullptr)) {
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
                phantomInfo.tileX, phantomInfo.tileY, TileLayer::Type::Wall,
                selectedSpriteSet->numericID, wallType});
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

    // If this tool is active and we have a selected sprite.
    if (isActive && (selectedSpriteSet != nullptr)) {
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
    TilePosition tilePosition{Transforms::screenToTile(screenPoint, camera)};
    Position tileTopLeft{
        static_cast<float>(tilePosition.x * SharedConfig::TILE_WORLD_WIDTH),
        static_cast<float>(tilePosition.y * SharedConfig::TILE_WORLD_WIDTH), 0};
    Position worldPosition{Transforms::screenToWorld(screenPoint, camera)};

    float relativeX{worldPosition.x - tileTopLeft.x};
    float relativeY{worldPosition.y - tileTopLeft.y};

    // If the mouse is closer to the top of the tile.
    if (relativeY < relativeX) {
        addNorthWallPhantom(tilePosition.x, tilePosition.y);
    }
    else {
        // The mouse is closer to the left side of the tile, or it's equal.
        addWestWallPhantom(tilePosition.x, tilePosition.y);
    }
}

void WallTool::addNorthWallPhantom(int tileX, int tileY)
{
    const Tile& tile{world.tileMap.getTile(tileX, tileY)};
    const std::array<WallTileLayer, 2>& walls{tile.getWalls()};

    // If the tile has a West wall, add a NE gap fill.
    if (walls[0].wallType == Wall::Type::West) {
        pushPhantomWall(tileX, tileY, Wall::Type::NorthEastGapFill,
                        *selectedSpriteSet);
    }
    else {
        // No West wall, add the North wall.
        // Note: If there's a NorthWestGapFill, this will replace it.
        pushPhantomWall(tileX, tileY, Wall::Type::North, *selectedSpriteSet);
    }

    // If there's a tile to the NE that we might've formed a corner with.
    const TileExtent& mapTileExtent{world.tileMap.getTileExtent()};
    if (mapTileExtent.containsPosition({tileX + 1, tileY - 1})) {
        const Tile& northeastTile{world.tileMap.getTile(tileX + 1, tileY - 1)};
        const auto& northeastWalls{northeastTile.getWalls()};

        // If the NorthEast tile has a West wall.
        if (northeastWalls[0].wallType == Wall::Type::West) {
            // We formed a corner. Check if the tile to the east has a wall.
            // Note: We know this tile is valid cause there's a NorthEast tile.
            const Tile& eastTile{world.tileMap.getTile(tileX + 1, tileY)};
            const std::array<WallTileLayer, 2>& eastWalls{eastTile.getWalls()};
            if ((eastWalls[0].wallType == Wall::Type::None)
                && (eastWalls[1].wallType == Wall::Type::None)) {
                // The tile has no walls. Add a NorthWestGapFill.
                pushPhantomWall(tileX + 1, tileY, Wall::Type::NorthWestGapFill,
                                *selectedSpriteSet);
            }
            else if (eastWalls[1].wallType == Wall::Type::NorthWestGapFill) {
                // The East tile has a NW gap fill. If its sprite set no longer
                // matches either surrounding wall, make it match the new wall.
                int gapFillID{eastWalls[1].spriteSet->numericID};
                int newNorthID{selectedSpriteSet->numericID};
                int westID{northeastWalls[0].spriteSet->numericID};
                if ((gapFillID != newNorthID) && (gapFillID != westID)) {
                    pushPhantomWall(tileX + 1, tileY,
                                    Wall::Type::NorthWestGapFill,
                                    *selectedSpriteSet);
                }
            }
        }
    }
}

void WallTool::addWestWallPhantom(int tileX, int tileY)
{
    const Tile& tile{world.tileMap.getTile(tileX, tileY)};
    const std::array<WallTileLayer, 2>& walls{tile.getWalls()};

    // Add the West wall.
    // Note: If this tile already has a West wall or NW gap fill, this will
    //       replace it.
    pushPhantomWall(tileX, tileY, Wall::Type::West, *selectedSpriteSet);

    // If the tile has a North wall, switch it to a NorthEast gap fill.
    if (walls[1].wallType == Wall::Type::North) {
        pushPhantomWall(tileX, tileY, Wall::Type::NorthEastGapFill,
                        *(walls[1].spriteSet));
    }

    // If there's a tile to the SW that we might've formed a corner with.
    const TileExtent& mapTileExtent{world.tileMap.getTileExtent()};
    if (mapTileExtent.containsPosition({tileX - 1, tileY + 1})) {
        const Tile& southwestTile{world.tileMap.getTile(tileX - 1, tileY + 1)};
        const auto& southwestWalls{southwestTile.getWalls()};

        // If the SouthWest tile has a North wall or a NE gap fill.
        if ((southwestWalls[1].wallType == Wall::Type::North)
            || (southwestWalls[1].wallType == Wall::Type::NorthEastGapFill)) {
            // We formed a corner. Check if the tile to the South has a wall.
            // Note: We know this tile is valid cause there's a SouthWest tile.
            const Tile& southTile{world.tileMap.getTile(tileX, tileY + 1)};
            const std::array<WallTileLayer, 2>& southWalls{
                southTile.getWalls()};
            if ((southWalls[0].wallType == Wall::Type::None)
                && (southWalls[1].wallType == Wall::Type::None)) {
                // The tile has no walls. Add a NorthWestGapFill.
                pushPhantomWall(tileX, tileY + 1, Wall::Type::NorthWestGapFill,
                                *selectedSpriteSet);
            }
            else if (southWalls[1].wallType == Wall::Type::NorthWestGapFill) {
                // The South tile has a NW gap fill. If its sprite set no longer
                // matches either surrounding wall, make it match the new wall.
                int gapFillID{southWalls[1].spriteSet->numericID};
                int newWestID{selectedSpriteSet->numericID};
                int northID{southwestWalls[1].spriteSet->numericID};
                if ((gapFillID != newWestID) && (gapFillID != northID)) {
                    pushPhantomWall(tileX, tileY + 1,
                                    Wall::Type::NorthWestGapFill,
                                    *selectedSpriteSet);
                }
            }
        }
    }
}

void WallTool::pushPhantomWall(int tileX, int tileY, Wall::Type wallType,
                               const WallSpriteSet& wallSpriteSet)
{
    phantomSprites.emplace_back(tileX, tileY, TileLayer::Type::Wall, wallType,
                                Position{},
                                &(wallSpriteSet.sprites[wallType].get()));
}

} // End namespace Client
} // End namespace AM
