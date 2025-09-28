#include "MazeGenerationSystem.h"
#include "World.h"
#include "GraphicData.h"
#include "MazeTopology.h"
#include "Position.h"
#include "Collision.h"
#include "Log.h"
#include "AMAssert.h"
#include <algorithm>

namespace AM
{
namespace Server
{
const TilePosition MazeGenerationSystem::MAZE_ORIGIN_TILE{16, 18, 0};

MazeGenerationSystem::MazeGenerationSystem(World& inWorld,
                                           GraphicData& inGraphicData)
: world{inWorld}
, graphicData{inGraphicData}
, regenerationTimer{}
, mazeExtent{MAZE_ORIGIN_TILE.x, MAZE_ORIGIN_TILE.y, MAZE_ORIGIN_TILE.z,
             MAZE_WIDTH,         MAZE_HEIGHT,        1}
, abstractMazeExtent{0,
                     0,
                     0,
                     (mazeExtent.xLength / 2),
                     (mazeExtent.yLength / 2),
                     1}
, entranceTile{8, 17, 0}
, exitTiles{{1, 0, 0}, {0, 11, 0}, {17, 2, 0}}
, workingPath{}
, workingNeighbors{}
, randGenerator{std::random_device()()}
{
    // Fill in the graphic set ID's.
    //wallIDs[0] = graphicData.getWallGraphicSet("hedge").numericID;
    //wallIDs[1] = graphicData.getWallGraphicSet("hedgeflower").numericID;
    //wallIDs[2] = graphicData.getWallGraphicSet("roundedhedge").numericID;
    //wallIDs[3] = graphicData.getWallGraphicSet("roundedhedgeflower").numericID;
    //wallIDs[4] = graphicData.getWallGraphicSet("squaredhedge").numericID;
    //wallIDs[5] = graphicData.getWallGraphicSet("squaredhedgeflower").numericID;
    //fullFillID = graphicData.getObjectGraphicSet("hedgefullfill").numericID;

    // Prime a timer.
    Timer timer;

    // Generate the initial maze state.
    //MazeTopology maze{};
    //generateMaze(maze);
    //applyMazeToMap(maze);

    LOG_INFO("Maze generated in %.8fs", timer.getTime());
}

void MazeGenerationSystem::regenerateMazeIfNecessary()
{
    //// If enough time has passed, regenerate the maze.
    //if (regenerationTimer.getTime() >= MAZE_REGENERATION_PERIOD_S) {
    //    LOG_INFO("Generating maze...");

    //    // Prime a timer.
    //    Timer timer{};

    //    // Generate the maze topology.
    //    MazeTopology maze{};
    //    generateMaze(maze);

    //    // Apply the generated maze to the map.
    //    applyMazeToMap(maze);

    //    LOG_INFO("Maze generated in %.8fs", timer.getTime());

    //    regenerationTimer.reset();
    //}
}

void MazeGenerationSystem::generateMaze(MazeTopology& outMaze)
{
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

    // For each entity in the maze, clear to the existing path or an exit.
    const auto& collisionResult{world.collisionLocator.getCollisions(
        mazeExtent, (CollisionLayerType::ClientEntity
                     | CollisionLayerType::NonClientEntity))};
    for (const auto* collisionInfo : collisionResult) {
        // Calc the tile that the entity's center is on.
        TilePosition tilePosition(
            collisionInfo->collisionVolume.getBottomCenterPoint());
        TilePosition abstractTilePosition{(tilePosition.x - mazeExtent.x) / 2,
                                          (tilePosition.y - mazeExtent.y) / 2,
                                          0};

        // Clear a path to the existing path or another exit.
        clearToVisitedOrExit(outMaze, abstractTilePosition, passNumber++);

        // Clear any tiles that the entity is touching.
        clearTilesTouchingEntity(outMaze, collisionInfo->entity);
    }

    // Flag any fully-enclosed cells to use the "full fill" sprite.
    fillEnclosedCells(outMaze);
}

void MazeGenerationSystem::clearToExit(MazeTopology& maze,
                                       const TilePosition& startPosition,
                                       int passNumber)
{
    // Note: Uses a "backtracking generator" maze algorithm.

    // Clear the working vector and start tracking our path.
    workingPath.clear();
    workingPath.push_back(startPosition);

    // Find an exit.
    while (true) {
        // Get a valid neighboring tile to move to.
        workingNeighbors.clear();
        getNeighboringTiles(maze, workingPath, false, passNumber,
                            workingNeighbors);

        // If there are valid neighbors: pick a random one, remove the wall,
        // and move to it.
        if (workingNeighbors.size() > 0) {
            clearAndMoveToRandomNeighbor(maze, workingNeighbors, workingPath,
                                         passNumber);
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
        }

        // If the current tile is an exit other than the one we started on,
        // we're done.
        if ((workingPath.back() != workingPath.front())
            && isExitTile(workingPath.back())) {
            return;
        }
    }
}

void MazeGenerationSystem::clearToVisitedOrExit(
    MazeTopology& maze, const TilePosition& startPosition, int passNumber)
{
    // Note: Uses a "backtracking generator" maze algorithm.

    // Clear the working vector and start tracking our path.
    workingPath.clear();
    workingPath.push_back(startPosition);

    // Find an already-visited cell, or an exit.
    while (true) {
        // Get a valid neighboring tile to move to.
        workingNeighbors.clear();
        getNeighboringTiles(maze, workingPath, true, passNumber,
                            workingNeighbors);

        // If any of the neighbors were already visited, we're done.
        if (clearToNeighborIfVisited(maze, workingNeighbors, workingPath,
                                     passNumber)) {
            return;
        }

        // If there are valid neighbors: pick a random one, remove the wall,
        // and move to it.
        if (workingNeighbors.size() > 0) {
            clearAndMoveToRandomNeighbor(maze, workingNeighbors, workingPath,
                                         passNumber);
        }
        else {
            // There were no valid neighbors, backtrack.
            workingPath.pop_back();
        }

        // If the current tile is an exit other than the one we started on,
        // we're done.
        if ((workingPath.back() != workingPath.front())
            && isExitTile(workingPath.back())) {
            return;
        }
    }
}

void MazeGenerationSystem::getNeighboringTiles(
    const MazeTopology& maze, const std::vector<TilePosition>& path,
    bool includeVisited, int passNumber,
    std::vector<TilePosition>& outNeighbors)
{
    // Add the coordinates that are directly N,S,E,W of the given position.
    const TilePosition& currentPosition{path.back()};
    outNeighbors.emplace_back(currentPosition.x + 1, currentPosition.y, 0);
    outNeighbors.emplace_back(currentPosition.x - 1, currentPosition.y, 0);
    outNeighbors.emplace_back(currentPosition.x, currentPosition.y + 1, 0);
    outNeighbors.emplace_back(currentPosition.x, currentPosition.y - 1, 0);

    // Remove any invalid neighbors.
    std::erase_if(outNeighbors, [&](const TilePosition& position) {
        // If this neighbor is out of bounds, remove it.
        if (!(abstractMazeExtent.contains(position))) {
            return true;
        }
        // Else if we're including visited tiles.
        else if (includeVisited && (path.size() > 1)) {
            // If this tile was already visited on this run, remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(position.x, position.y)]};
            if (cell.lastVisitedPassNumber == passNumber) {
                return true;
            }
        }
        else {
            // If we're ignoring visited cells and this neighbor was
            // visited, remove it.
            const MazeCell& cell{
                maze.cells[linearizeCellIndex(position.x, position.y)]};
            if (!includeVisited && (cell.lastVisitedPassNumber != -1)) {
                return true;
            }
        }

        return false;
    });
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

void MazeGenerationSystem::clearAndMoveToRandomNeighbor(
    MazeTopology& maze, const std::vector<TilePosition>& neighbors,
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

bool MazeGenerationSystem::clearToNeighborIfVisited(
    MazeTopology& maze, const std::vector<TilePosition>& neighbors,
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

void MazeGenerationSystem::fillEnclosedCells(MazeTopology& maze)
{
    // Iterate through the maze.
    for (int mazeX = 0; mazeX <= abstractMazeExtent.xMax(); ++mazeX) {
        for (int mazeY = 0; mazeY <= abstractMazeExtent.yMax(); ++mazeY) {
            MazeCell& cell{maze.cells[linearizeCellIndex(mazeX, mazeY)]};

            // Check if this cell has a north wall or a fill to the north.
            bool hasNorthWall{cell.northWall};
            if (!hasNorthWall && (mazeY > 0)) {
                MazeCell& northCell{
                    maze.cells[linearizeCellIndex(mazeX, (mazeY - 1))]};
                hasNorthWall = northCell.fullFill;
            }

            // Check if this cell has a west wall or a fill to the west.
            bool hasWestWall{cell.westWall};
            if (!hasWestWall && (mazeX > 0)) {
                MazeCell& westCell{
                    maze.cells[linearizeCellIndex((mazeX - 1), mazeY)]};
                hasWestWall = westCell.fullFill;
            }

            // If there's a cell to the east, check if it has a west wall.
            // Note: If there's no cell to the east, we're on the eastern
            //       boundary of the maze and know there's an enclosing wall.
            bool onEastBoundary{true};
            bool eastCellHasWestWall{false};
            if (mazeX < abstractMazeExtent.xMax()) {
                onEastBoundary = false;
                MazeCell& eastCell{
                    maze.cells[linearizeCellIndex((mazeX + 1), mazeY)]};
                eastCellHasWestWall = eastCell.westWall;
            }

            // If there's a cell to the south, check if it has a north wall.
            // Note: If there's no cell to the south, we're on the southern
            //       boundary of the maze and know there's an enclosing wall.
            bool onSouthBoundary{true};
            bool southCellHasNorthWall{false};
            if (mazeY < abstractMazeExtent.yMax()) {
                onSouthBoundary = false;
                MazeCell& southCell{
                    maze.cells[linearizeCellIndex(mazeX, (mazeY + 1))]};
                southCellHasNorthWall = southCell.northWall;
            }

            // If this cell is fully enclosed by walls, remove them and place
            // a full fill.
            if (hasNorthWall && hasWestWall
                && (onEastBoundary || eastCellHasWestWall)
                && (onSouthBoundary || southCellHasNorthWall)) {
                cell.northWall = false;
                cell.westWall = false;

                if (eastCellHasWestWall) {
                    MazeCell& eastCell{
                        maze.cells[linearizeCellIndex((mazeX + 1), mazeY)]};
                    eastCell.westWall = false;
                }
                if (southCellHasNorthWall) {
                    MazeCell& southCell{
                        maze.cells[linearizeCellIndex(mazeX, (mazeY + 1))]};
                    southCell.northWall = false;
                }

                cell.fullFill = true;
            }
        }
    }
}

void MazeGenerationSystem::applyMazeToMap(const MazeTopology& maze)
{
    // Disable auto collision rebuild (it's more efficient to do it all after).
    world.tileMap.setAutoRebuildCollision(false);

    // Clear the maze area in the tile map.
    world.tileMap.clearExtentLayers(mazeExtent, {TileLayer::Type::Floor,
                                                 TileLayer::Type::Wall,
                                                 TileLayer::Type::Object});

    // Apply the maze to the tile map.
    for (int mazeX{0}; mazeX <= abstractMazeExtent.xMax(); ++mazeX) {
        for (int mazeY{0}; mazeY <= abstractMazeExtent.yMax(); ++mazeY) {
            const MazeCell& cell{maze.cells[linearizeCellIndex(mazeX, mazeY)]};

            // Apply this cell's walls to the corresponding 2x2 map area.
            int mapX{mazeExtent.x + (mazeX * 2)};
            int mapY{mazeExtent.y + (mazeY * 2)};
            applyCellToMap(mapX, mapY, cell);
        }
    }

    // Re-enable auto collision rebuild (rebuilds any dirty tiles).
    world.tileMap.setAutoRebuildCollision(true);
}

void MazeGenerationSystem::applyCellToMap(int mapX, int mapY,
                                          const MazeCell& cell)
{
    // Determine which walls this cell has and apply them to the map.
    if (cell.fullFill) {
        // Fully-filled cell. Place the 4 fills.
        world.tileMap.addObject({mapX, mapY, 0}, {}, fullFillID,
                                getRandomFullFill());
        world.tileMap.addObject({(mapX + 1), mapY, 0}, {}, fullFillID,
                                getRandomFullFill());
        world.tileMap.addObject({mapX, (mapY + 1), 0}, {}, fullFillID,
                                getRandomFullFill());
        world.tileMap.addObject({(mapX + 1), (mapY + 1), 0}, {}, fullFillID,
                                getRandomFullFill());
    }
    else if (cell.northWall && cell.westWall) {
        // Northwest corner, place the 2 west walls and 2 north walls.
        world.tileMap.addWall({mapX, mapY, 0}, getRandomWall(),
                              Wall::Type::West);
        world.tileMap.addWall({mapX, (mapY + 1), 0}, getRandomWall(),
                              Wall::Type::West);
        world.tileMap.addWall({mapX, mapY, 0}, getRandomWall(),
                              Wall::Type::North);
        world.tileMap.addWall({(mapX + 1), mapY, 0}, getRandomWall(),
                              Wall::Type::North);
    }
    else if (cell.northWall) {
        // North only, place the 2 north walls.
        world.tileMap.addWall({mapX, mapY, 0}, getRandomWall(),
                              Wall::Type::North);
        world.tileMap.addWall({(mapX + 1), mapY, 0}, getRandomWall(),
                              Wall::Type::North);
    }
    else if (cell.westWall) {
        // West only, place the 2 west walls.
        world.tileMap.addWall({mapX, mapY, 0}, getRandomWall(),
                              Wall::Type::West);
        world.tileMap.addWall({mapX, (mapY + 1), 0}, getRandomWall(),
                              Wall::Type::West);
    }
}

void MazeGenerationSystem::clearTilesTouchingEntity(MazeTopology& maze,
                                                    const entt::entity entity)
{
    // Get the tile extent that this entity is touching, clip it to the
    // maze bounds, and make it relative to the maze origin.
    const Collision& collision{world.registry.get<Collision>(entity)};
    TileExtent tileExtent(collision.worldBounds);
    tileExtent = tileExtent.intersectWith(mazeExtent);
    tileExtent.x -= mazeExtent.x;
    tileExtent.y -= mazeExtent.y;

    // Convert the extent so we can work with the abstract representation.
    TilePosition topLeft{};
    topLeft.x = tileExtent.x / 2;
    topLeft.y = tileExtent.y / 2;

    TilePosition bottomRight{};
    bottomRight.x = static_cast<int>(
        std::ceil((tileExtent.x + tileExtent.xLength) / 2.f));
    bottomRight.y = static_cast<int>(
        std::ceil((tileExtent.y + tileExtent.yLength) / 2.f));

    const TileExtent abstractTileExtent{topLeft.x,
                                        topLeft.y,
                                        0,
                                        (bottomRight.x - topLeft.x),
                                        (bottomRight.y - topLeft.y),
                                        1};

    // Clear all walls from any tiles that the entity is touching.
    for (int x = abstractTileExtent.x; x <= abstractTileExtent.xMax(); ++x) {
        for (int y = abstractTileExtent.y; y <= abstractTileExtent.yMax();
             ++y) {
            MazeCell& cell{maze.cells[linearizeCellIndex(x, y)]};
            // Note: We check for x/y == 0 to preserve the surrounding walls.
            if ((y != 0) && cell.northWall) {
                cell.northWall = false;
            }
            if ((x != 0) && cell.westWall) {
                cell.westWall = false;
            }
        }
    }
}

Uint16 MazeGenerationSystem::getRandomWall()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 70) {
        return wallIDs[4];
    }
    else if (randValue >= 70 && randValue < 90) {
        return wallIDs[5];
    }
    else if (randValue >= 90 && randValue < 93) {
        return wallIDs[2];
    }
    else if (randValue >= 93 && randValue < 96) {
        return wallIDs[3];
    }
    else if (randValue >= 96 && randValue < 98) {
        return wallIDs[0];
    }
    else {
        return wallIDs[1];
    }
}

Rotation::Direction MazeGenerationSystem::getRandomFullFill()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 60) {
        return static_cast<Rotation::Direction>(0);
    }
    else if (randValue >= 60 && randValue < 80) {
        return static_cast<Rotation::Direction>(1);
    }
    else {
        return static_cast<Rotation::Direction>(2);
    }
}

} // End namespace Server
} // End namespace AM
