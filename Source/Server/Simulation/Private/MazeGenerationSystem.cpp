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
    // TODO: Why are we getting walls in the middle of the entrance?
    //       Happens to exits too
    //       Probably cause they're on odd numbers. Move them to match

    // Allocate memory for the temporary abstract maze.
    outMaze.cells.resize(
        abstractMazeExtent.xLength * abstractMazeExtent.yLength, MazeCell{});

    // Clear out the north and west exit walls (south and east are fine).
    MazeCell& northExit{
        outMaze.cells[linearizeCellIndex(exitTiles[0].x, exitTiles[0].y)]};
    northExit.northWall = false;
    MazeCell& westExit{
        outMaze.cells[linearizeCellIndex(exitTiles[1].x, exitTiles[1].y)]};
    westExit.westWall = false;

    // Clear a path from the entrance to an exit.
    int passNumber{0};
    clearToExit(outMaze, entranceTile, passNumber++);

    // Clear a path from each exit to the existing path or another exit.
    for (const TilePosition& exitTile : exitTiles) {
        clearToVisitedOrExit(outMaze, exitTile, passNumber++);
    }

    // Clear a path from each player's position to the existing path.
}

void MazeGenerationSystem::clearToExit(MazeTopology& maze, const TilePosition& startPosition, int passNumber)
{
    // Note: Uses a "backtracking generator" maze algorithm.

    // Clear the working vector and start tracking our path.
    workingPath.clear();
    workingPath.push_back(startPosition);

    // Find an exit.
    while (true) {
        // Get a valid neighboring tile to move to.
        workingNeighbors.clear();
        getNeighboringTiles(maze, workingPath, false, passNumber, workingNeighbors);

        // If there's a valid neighbor, remove the wall and move to it.
        if (workingNeighbors.size() > 0) {
            clearAndMoveToRandomNeighbor(maze, workingNeighbors, workingPath,
                                         passNumber);
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
        }

        // If the current tile is an exit, we're done.
        if (isExitTile(workingPath.back())) {
            return;
        }
    }
}

void MazeGenerationSystem::clearToVisitedOrExit(MazeTopology& maze
    , const TilePosition& startPosition, int passNumber)
{
    // Note: Uses a "backtracking generator" maze algorithm.

    // Clear the working vector and start tracking our path.
    workingPath.clear();
    workingPath.push_back(startPosition);

    // Find an already-visited cell, or an exit.
    while (true) {
        // Get a valid neighboring tile to move to.
        workingNeighbors.clear();
        getNeighboringTiles(maze, workingPath, true, passNumber, workingNeighbors);

        // If any of the neighbors were already visited, we're done.
        if (clearToNeighborIfVisited(maze, workingNeighbors, workingPath, passNumber)) {
            return;
        }

        // If there's a valid neighbor, remove the wall and move to it.
        if (workingNeighbors.size() > 0) {
            clearAndMoveToRandomNeighbor(maze, workingNeighbors, workingPath, passNumber);
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
        }

        // If the current tile is an exit, we're done.
        if (isExitTile(workingPath.back())) {
            return;
        }
    }
}

void MazeGenerationSystem::getNeighboringTiles(const MazeTopology& maze,
                         const std::vector<TilePosition>& path, bool includeVisited, int passNumber,
                         std::vector<TilePosition>& outNeighbors)
{
    // Add the coordinates that are directly N,S,E,W of the given position.
    const TilePosition& currentPosition{path.back()};
    outNeighbors.emplace_back(currentPosition.x + 1, currentPosition.y);
    outNeighbors.emplace_back(currentPosition.x - 1, currentPosition.y);
    outNeighbors.emplace_back(currentPosition.x, currentPosition.y + 1);
    outNeighbors.emplace_back(currentPosition.x, currentPosition.y - 1);

    // Remove any invalid neighbors.
    for (auto it = outNeighbors.begin(); it != outNeighbors.end();) {
        bool removeNeighbor{false};

        // If this neighbor is out of bounds, remove it.
        if (!(abstractMazeExtent.containsPosition(*it))) {
            removeNeighbor = true;
        }
        // Else if we're including visited tiles.
        else if (includeVisited && (path.size() > 1)) {
            // If this tile was already visited on this run, remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(it->x, it->y)]};
            if (cell.lastVisitedPassNumber == passNumber) {
                removeNeighbor = true;
            }
        }
        else {
            // If we're ignoring visited cells and this neighbor was visited, 
            // remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(it->x, it->y)]};
            if (!includeVisited && (cell.lastVisitedPassNumber != -1)) {
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
                                 std::vector<TilePosition>& path, int passNumber)
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
    neighborCell.lastVisitedPassNumber = passNumber;
    path.push_back(chosenNeighbor);
}

bool MazeGenerationSystem::clearToNeighborIfVisited(MazeTopology& maze,
                              const std::vector<TilePosition>& neighbors,
                              std::vector<TilePosition>& path, int passNumber)
{
    // Check all the neighbors.
    for (const TilePosition& neighborTile : neighbors) {
        MazeCell& neighborCell{
            maze.cells[linearizeCellIndex(neighborTile.x, neighborTile.y)]};

        // If this neighbor was already visited, remove the wall.
        if (neighborCell.lastVisitedPassNumber != -1) {
            const TilePosition& currentTile{path.back()};
            MazeCell& currentCell{
                maze.cells[linearizeCellIndex(currentTile.x, currentTile.y)]};

            // Remove the wall.
            if (neighborTile.x < currentTile.x) {
                // West
                currentCell.westWall = false;
            }
            else if (neighborTile.x > currentTile.x) {
                // East
                neighborCell.westWall = false;
            }
            else if (neighborTile.y < currentTile.y) {
                // North
                currentCell.northWall = false;
            }
            else {
                // South
                neighborCell.northWall = false;
            }

            // Move to the neighbor.
            neighborCell.lastVisitedPassNumber = passNumber;
            path.push_back(neighborTile);

            return true;
        }
    }

    return false;
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
