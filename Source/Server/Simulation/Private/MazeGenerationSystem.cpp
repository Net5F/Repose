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
    if (regenerationTimer.getDeltaSeconds(false)
        >= MAZE_REGENERATION_PERIOD_S) {
        // Generate the maze topology.
        MazeTopology maze{};
        generateMaze(maze);

        // Apply the generated maze to the map.
        applyMazeToMap(maze);

        LOG_INFO("Regenerated maze");

        regenerationTimer.updateSavedTime();
    }
}

void MazeGenerationSystem::generateMaze(MazeTopology& outMaze)
{
    // Allocate memory for the new maze.
    // Note: Since we want the hallways to be 2 tiles wide, we generate the 
    //       maze at half size and double it at the end.
    unsigned int mazeWidth{static_cast<unsigned int>(mazeExtent.xLength / 2)};
    unsigned int mazeHeight{static_cast<unsigned int>(mazeExtent.yLength / 2)};
    outMaze.cells.resize(mazeWidth * mazeHeight);

    // Clear a path from the entrance to an exit.
    clearToExit(outMaze, entranceTile);

    // Clear a path from each exit to the existing path.
    for (const TilePosition& exitTile : exitTiles) {
        clearToVisited(outMaze, exitTile);
    }

    // Clear a path from each player's position to the existing path.
}

void MazeGenerationSystem::applyMazeToMap(const MazeTopology& maze)
{
    // Clear the maze area in the tile map.
    world.tileMap.clearExtent(mazeExtent, 1);

    // Apply the maze to the tile map.
    unsigned int mazeMaxX{static_cast<unsigned int>(mazeExtent.xLength / 2)};
    unsigned int mazeMaxY{static_cast<unsigned int>(mazeExtent.yLength / 2)};
    for (unsigned int mazeX = 0; mazeX < mazeMaxX; ++mazeX) {
        for (unsigned int mazeY = 0; mazeY < mazeMaxY; ++mazeY) {
            unsigned int mapStartX{mazeExtent.x + (mazeX * 2)};
            unsigned int mapStartY{mazeExtent.y + (mazeY * 2)};
            const MazeCell& cell{maze.cells[linearizeCellIndex(mazeX, mazeY)]};

            // Apply this cell's walls to the corresponding 2x2 map area.
            applyCellToMap(mapStartX, mapStartY, cell);
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
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
        }

        // If the current tile is an exit, we're done.
        if (isExitTile(workingPath.back())) {
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
        if (!(mazeExtent.containsPosition(position))) {
            removeNeighbor = true;
        }
        else {
            // If we're ignoring visited cells and this neighbor was visited, 
            // remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(position.x, position.y)]};
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
    std::uniform_int_distribution<std::size_t> dist{0, neighbors.size()};

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

void MazeGenerationSystem::applyCellToMap(unsigned int mapStartX, unsigned int mapStartY,
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
        world.tileMap.setTileSpriteLayer(mapStartX, mapStartY, 1, westWallID);
        world.tileMap.setTileSpriteLayer(mapStartX, (mapStartY + 1), 1,
                                         westWallID);
        world.tileMap.setTileSpriteLayer((mapStartX + 1), mapStartY, 1,
                                         northWallID);
        world.tileMap.setTileSpriteLayer(mapStartX, mapStartY, 2,
                                         northeastGapFillID);
    }
    else if (cell.northWall) {
        // North only, place the 2 north walls.
        world.tileMap.setTileSpriteLayer(mapStartX, mapStartY, 1, northWallID);
        world.tileMap.setTileSpriteLayer((mapStartX + 1), mapStartY, 1,
                                         northWallID);
    }
    else if (cell.westWall) {
        // West only, place the 2 west walls.
        world.tileMap.setTileSpriteLayer(mapStartX, mapStartY, 1, westWallID);
        world.tileMap.setTileSpriteLayer(mapStartX, (mapStartY + 1), 1,
                                         westWallID);
    }
    else {
        // No walls. First check if there are tiles to the north and west.
        if ((mapStartX > 0) && (mapStartY > 0)) {
            // There are tiles to the north and west, check if they have walls 
            // that form a gap.
            const Tile& northTile{
                world.tileMap.getTile(mapStartX, (mapStartY - 1))};
            const Tile& westTile{
                world.tileMap.getTile((mapStartX - 1), mapStartY)};

            bool northTileHasWestWall{
                (northTile.spriteLayers.size() > 1)
                && (northTile.spriteLayers[1].sprite.numericID == westWallID)};
            bool westTileHasNorthWall{
                (westTile.spriteLayers.size() > 1)
                && (westTile.spriteLayers[1].sprite.numericID == northWallID)};

            // If there's a gap, fill it.
            if (northTileHasWestWall && westTileHasNorthWall) {
                world.tileMap.setTileSpriteLayer(mapStartX, mapStartY, 1,
                                                 northwestGapFillID);
            }
        }
    }
}

} // End namespace Server
} // End namespace AM
