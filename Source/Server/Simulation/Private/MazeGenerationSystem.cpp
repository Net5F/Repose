#include "MazeGenerationSystem.h"
#include "World.h"
#include "SpriteData.h"
#include "MazeTopology.h"
#include <algorithm>

namespace AM
{
namespace Server
{

MazeGenerationSystem::MazeGenerationSystem(World& inWorld, SpriteData& inSpriteData)
: world{inWorld}
, spriteData{inSpriteData}
, mazeExtent{MAZE_ORIGIN_TILE.x, MAZE_ORIGIN_TILE.y,
             MAZE_WIDTH, MAZE_HEIGHT}
, abstractMazeExtent{0, 0, (mazeExtent.xLength / 2), (mazeExtent.yLength / 2)}
, entranceTile{8, 17}
, exitTiles{{1, 0}, {0, 11}, {17, 2}}
, workingPath{}
, workingNeighbors{}
, randGenerator{std::random_device()()}
{
    // Generate the initial maze state.
    MazeTopology maze{};
    generateMaze(maze);
    applyMazeToMap(maze);

    regenerationTimer.updateSavedTime();
}

void MazeGenerationSystem::regenerateMazeIfNecessary()
{
    // If enough time has passed, regenerate the maze.
    //if (regenerationTimer.getDeltaSeconds(false)
    //    >= MAZE_REGENERATION_PERIOD_S) {
    //    // Generate the maze topology.
    //    MazeTopology maze{};
    //    generateMaze(maze);

    //    // Apply the generated maze to the map.
    //    applyMazeToMap(maze);

    //    LOG_INFO("Regenerated maze");

    //    regenerationTimer.updateSavedTime();
    //}
}

void MazeGenerationSystem::generateMaze(MazeTopology& outMaze)
{
    // Allocate memory for the new maze.
    outMaze.cells.resize(
        abstractMazeExtent.xLength * abstractMazeExtent.yLength, MazeCell{});

    // Clear out the north and west exit walls (south and east are fine).
    MazeCell& northExit{
        outMaze.cells[linearizeCellIndex(exitTiles[0].x, exitTiles[0].y)]};
    northExit.northWall = false;
    MazeCell& westExit{
        outMaze.cells[linearizeCellIndex(exitTiles[1].x, exitTiles[1].y)]};
    northExit.westWall = false;

    // Clear a path from the entrance to an exit.
    clearToExit(outMaze, entranceTile);

    // Clear a path from each exit to the existing path.
    //for (const TilePosition& exitTile : exitTiles) {
    //    clearToVisited(outMaze, exitTile);
    //}

    // Clear a path from each player's position to the existing path.
}

void MazeGenerationSystem::applyMazeToMap(const MazeTopology& maze)
{
    // Clear the maze area in the tile map.
    world.tileMap.clearExtent(mazeExtent, 1);

    // Apply the maze to the tile map.
    int mazeMaxX{abstractMazeExtent.xLength};
    int mazeMaxY{abstractMazeExtent.yLength};
    for (int mazeX = 0; mazeX < mazeMaxX; ++mazeX) {
        for (int mazeY = 0; mazeY < mazeMaxY; ++mazeY) {
            int mapX{mazeExtent.x + (mazeX * 2)};
            int mapY{mazeExtent.y + (mazeY * 2)};
            const MazeCell& cell{maze.cells[linearizeCellIndex(mazeX, mazeY)]};

            // Apply this cell's walls to the corresponding 2x2 map area.
            applyCellToMap(mapX, mapY, cell);
        }
    }
}

void MazeGenerationSystem::clearToExit(MazeTopology& maze, const TilePosition& startPosition)
{
    // Note: Uses a "backtracking generator" maze algorithm.

    // Clear the working vector and start tracking the path we've taken.
    workingPath.clear();
    workingPath.push_back(startPosition);

    // Find an exit.
    bool exitFound{false};
    while (!exitFound) {
        // Get a valid neighboring tile to move to.
        workingNeighbors.clear();
        getNeighboringTiles(maze, workingPath.back(), false, workingNeighbors);

        // If there's a valid neighbor, remove the wall and move to it.
        if (workingNeighbors.size() > 0) {
            clearAndMoveToRandomNeighbor(maze, workingNeighbors, workingPath);
            LOG_INFO("Moved to: %d, %d", workingPath.back().x, workingPath.back().y);
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
            LOG_INFO("Backtrack: %d, %d", workingPath.back().x, workingPath.back().y);
        }

        // If the current tile is an exit, we're done.
        if (isExitTile(workingPath.back())) {
            LOG_INFO("Exit found: %d, %d", workingPath.back().x, workingPath.back().y);
            exitFound = true;
        }
    }
}

void MazeGenerationSystem::clearToVisited(MazeTopology& maze, const TilePosition& startPosition)
{
    // Note: Uses a "backtracking generator" maze algorithm.
}

void MazeGenerationSystem::getNeighboringTiles(const MazeTopology& maze,
                         const TilePosition& position, bool includeVisited,
                         std::vector<TilePosition>& outNeighbors)
{
    // Add the coordinates that are directly N,S,E,W of the given position.
    outNeighbors.emplace_back(position.x + 1, position.y);
    outNeighbors.emplace_back(position.x - 1, position.y);
    outNeighbors.emplace_back(position.x, position.y + 1);
    outNeighbors.emplace_back(position.x, position.y - 1);

    // Remove any invalid neighbors.
    for (auto it = outNeighbors.begin(); it != outNeighbors.end();) {
        bool removeNeighbor{false};

        // If this neighbor is out of bounds, remove it.
        if (!(abstractMazeExtent.containsPosition(*it))) {
            removeNeighbor = true;
        }
        else {
            // If we're ignoring visited cells and this neighbor was visited, 
            // remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(it->x, it->y)]};
            if (!includeVisited && cell.wasVisited) {
                removeNeighbor = true;
            }
        }

        if (removeNeighbor) {
            std::swap(*it, outNeighbors.back());
            outNeighbors.pop_back();
        }
        else {
            ++it;
        }
    }
}

bool MazeGenerationSystem::isExitTile(const TilePosition& position)
{
    auto it{std::find(exitTiles.begin(), exitTiles.end(), position)};
    if (it != exitTiles.end()) {
        return true;
    }
    else {
        return false;
    }
}

void
    MazeGenerationSystem::clearAndMoveToRandomNeighbor(MazeTopology& maze,
                                 const std::vector<TilePosition>& neighbors,
                                 std::vector<TilePosition>& path)
{
    // Choose a random neighbor and prepare the cells we'll be acting on.
    std::uniform_int_distribution<std::size_t> dist{0, (neighbors.size() - 1)};

    const TilePosition& chosenNeighbor{neighbors[dist(randGenerator)]};
    MazeCell& neighborCell{
        maze.cells[linearizeCellIndex(chosenNeighbor.x, chosenNeighbor.y)]};

    const TilePosition& currentTile{path.back()};
    MazeCell& currentCell{
        maze.cells[linearizeCellIndex(currentTile.x, currentTile.y)]};

    // Remove the wall.
    if (chosenNeighbor.x < currentTile.x) {
        // West
        currentCell.westWall = false;
    }
    else if (chosenNeighbor.x > currentTile.x) {
        // East
        neighborCell.westWall = false;
    }
    else if (chosenNeighbor.y < currentTile.y) {
        // North
        currentCell.northWall = false;
    }
    else {
        // South
        neighborCell.northWall = false;
    }

    // Move to the neighbor.
    neighborCell.wasVisited = true;
    path.push_back(chosenNeighbor);
}

void MazeGenerationSystem::applyCellToMap(int mapX, int mapY,
                    const MazeCell& cell)
{
    int northWallID{spriteData.get("wall_2").numericID};
    int westWallID{spriteData.get("wall_0").numericID};
    int northeastGapFillID{spriteData.get("gap_fill_0").numericID};
    int northwestGapFillID{spriteData.get("gap_fill_1").numericID};

    // Determine which walls this cell has and apply them to the map.
    if (cell.northWall && cell.westWall) {
        // Northwest corner, place the 2 west walls, the north wall, and the 
        // northeast gap fill.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, westWallID);
        world.tileMap.setTileSpriteLayer(mapX, (mapY + 1), 1,
                                         westWallID);
        world.tileMap.setTileSpriteLayer((mapX + 1), mapY, 1,
                                         northWallID);
        world.tileMap.setTileSpriteLayer(mapX, mapY, 2,
                                         northeastGapFillID);
    }
    else if (cell.northWall) {
        // North only, place the 2 north walls.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, northWallID);
        world.tileMap.setTileSpriteLayer((mapX + 1), mapY, 1,
                                         northWallID);
    }
    else if (cell.westWall) {
        // West only, place the 2 west walls.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, westWallID);
        world.tileMap.setTileSpriteLayer(mapX, (mapY + 1), 1,
                                         westWallID);
    }
    else {
        // No walls. First check if there are tiles to the north and west.
        if ((mapX > 0) && (mapY > 0)) {
            // There are tiles to the north and west, check if they have walls 
            // that form a gap.
            const Tile& northTile{
                world.tileMap.getTile(mapX, (mapY - 1))};
            const Tile& westTile{
                world.tileMap.getTile((mapX - 1), mapY)};

            bool northTileHasWestWall{
                (northTile.spriteLayers.size() > 1)
                && (northTile.spriteLayers[1].sprite.numericID == westWallID)};
            bool westTileHasNorthWall{
                (westTile.spriteLayers.size() > 1)
                && (westTile.spriteLayers[1].sprite.numericID == northWallID)};

            // If there's a gap, fill it.
            if (northTileHasWestWall && westTileHasNorthWall) {
                world.tileMap.setTileSpriteLayer(mapX, mapY, 1,
                                                 northwestGapFillID);
            }
        }
    }
}

} // End namespace Server
} // End namespace AM
