#include "MazeGenerationSystem.h"
#include "World.h"
#include "SpriteData.h"
#include "MazeTopology.h"
#include "Collision.h"
#include "Log.h"
#include "AMAssert.h"
#include <algorithm>

namespace AM
{
namespace Server
{

MazeGenerationSystem::MazeGenerationSystem(World& inWorld,
                                           SpriteData& inSpriteData)
: world{inWorld}
, spriteData{inSpriteData}
, mazeExtent{MAZE_ORIGIN_TILE.x, MAZE_ORIGIN_TILE.y, MAZE_WIDTH, MAZE_HEIGHT}
, abstractMazeExtent{0, 0, (mazeExtent.xLength / 2), (mazeExtent.yLength / 2)}
, entranceTile{8, 17}
, exitTiles{{1, 0}, {0, 11}, {17, 2}}
, workingPath{}
, workingNeighbors{}
, randGenerator{std::random_device()()}
{
    // Fill the sprite ID arrays.
    for (std::size_t i = 0; i < northWallIDs.size(); ++i) {
        std::string stringID{"north"};
        stringID += std::to_string(i + 1);
        northWallIDs[i] = spriteData.get(stringID).numericID;
    }
    for (std::size_t i = 0; i < westWallIDs.size(); ++i) {
        std::string stringID{"west"};
        stringID += std::to_string(i + 1);
        westWallIDs[i] = spriteData.get(stringID).numericID;
    }
    for (std::size_t i = 0; i < northeastFillIDs.size(); ++i) {
        std::string stringID{"ne_fill"};
        stringID += std::to_string(i + 1);
        northeastFillIDs[i] = spriteData.get(stringID).numericID;
    }
    for (std::size_t i = 0; i < northwestFillIDs.size(); ++i) {
        std::string stringID{"nw_fill"};
        stringID += std::to_string(i + 1);
        northwestFillIDs[i] = spriteData.get(stringID).numericID;
    }
    for (std::size_t i = 0; i < fullFillIDs.size(); ++i) {
        std::string stringID{"fullfill"};
        stringID += std::to_string(i + 1);
        fullFillIDs[i] = spriteData.get(stringID).numericID;
    }

    Timer timer;
    timer.updateSavedTime();

    // Generate the initial maze state.
    MazeTopology maze{};
    generateMaze(maze);
    applyMazeToMap(maze);

    regenerationTimer.updateSavedTime();

    LOG_INFO("Maze generated in %.8fs", timer.getDeltaSeconds(false));
}

void MazeGenerationSystem::regenerateMazeIfNecessary()
{
    // If enough time has passed, regenerate the maze.
    if (regenerationTimer.getDeltaSeconds(false)
        >= MAZE_REGENERATION_PERIOD_S) {
        LOG_INFO("Generating maze...");

        Timer timer;
        timer.updateSavedTime();

        // Generate the maze topology.
        MazeTopology maze{};
        generateMaze(maze);

        // Apply the generated maze to the map.
        applyMazeToMap(maze);

        regenerationTimer.updateSavedTime();

        LOG_INFO("Maze generated in %.8fs", timer.getDeltaSeconds(false));
    }
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
    const std::vector<entt::entity>& entitiesInMaze{
        world.entityLocator.getEntitiesFine(mazeExtent)};
    for (const entt::entity entity : entitiesInMaze) {
        // Calc the tile that the entity's center is on.
        const Position& position{world.registry.get<Position>(entity)};
        TilePosition tilePosition{position.asTilePosition()};
        TilePosition abstractTilePosition{(tilePosition.x - mazeExtent.x) / 2,
                                          (tilePosition.y - mazeExtent.y) / 2};

        // Clear a path to the existing path or another exit.
        clearToVisitedOrExit(outMaze, abstractTilePosition, passNumber++);

        // Clear any tiles that the entity is touching.
        clearTilesTouchingEntity(outMaze, entity);
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

        // If there's a valid neighbor, remove the wall and move to it.
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

        // If there's a valid neighbor, remove the wall and move to it.
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
            const MazeCell& cell{maze.cells[linearizeCellIndex(it->x, it->y)]};
            if (cell.lastVisitedPassNumber == passNumber) {
                removeNeighbor = true;
            }
        }
        else {
            // If we're ignoring visited cells and this neighbor was visited,
            // remove it.
            const MazeCell& cell{maze.cells[linearizeCellIndex(it->x, it->y)]};
            if (!includeVisited && (cell.lastVisitedPassNumber != -1)) {
                removeNeighbor = true;
            }
        }

        if (removeNeighbor) {
            // If we're at the end, just pop the back.
            if (it == outNeighbors.end() - 1) {
                outNeighbors.pop_back();
                it = outNeighbors.end();
            }
            else {
                // Not at the end, swap and pop.
                std::swap(*it, outNeighbors.back());
                outNeighbors.pop_back();
            }
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
    // Clear the maze area in the tile map.
    std::size_t endLayerIndex{SharedConfig::MAX_TILE_LAYERS - 1};
    world.tileMap.clearExtentSpriteLayers(mazeExtent, 1, endLayerIndex);

    // Apply the maze to the tile map.
    for (int mazeX = 0; mazeX <= abstractMazeExtent.xMax(); ++mazeX) {
        for (int mazeY = 0; mazeY <= abstractMazeExtent.yMax(); ++mazeY) {
            const MazeCell& cell{maze.cells[linearizeCellIndex(mazeX, mazeY)]};

            // Apply this cell's walls to the corresponding 2x2 map area.
            int mapX{mazeExtent.x + (mazeX * 2)};
            int mapY{mazeExtent.y + (mazeY * 2)};
            applyCellToMap(mapX, mapY, cell);
        }
    }
}

void MazeGenerationSystem::applyCellToMap(int mapX, int mapY,
                                          const MazeCell& cell)
{
    // Determine which walls this cell has and apply them to the map.
    if (cell.fullFill) {
        // Fully-filled cell. Place the 4 fills.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, getRandomFullFill());
        world.tileMap.setTileSpriteLayer((mapX + 1), mapY, 1,
                                         getRandomFullFill());
        world.tileMap.setTileSpriteLayer(mapX, (mapY + 1), 1,
                                         getRandomFullFill());
        world.tileMap.setTileSpriteLayer((mapX + 1), (mapY + 1), 1,
                                         getRandomFullFill());
    }
    else if (cell.northWall && cell.westWall) {
        // Northwest corner, place the 2 west walls, the north wall, and the
        // northeast gap fill.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, getRandomWestWall());
        world.tileMap.setTileSpriteLayer(mapX, (mapY + 1), 1,
                                         getRandomWestWall());
        world.tileMap.setTileSpriteLayer((mapX + 1), mapY, 1,
                                         getRandomNorthWall());
        world.tileMap.setTileSpriteLayer(mapX, mapY, 2, getRandomNEFill());
    }
    else if (cell.northWall) {
        // North only, place the 2 north walls.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, getRandomNorthWall());
        world.tileMap.setTileSpriteLayer((mapX + 1), mapY, 1,
                                         getRandomNorthWall());
    }
    else if (cell.westWall) {
        // West only, place the 2 west walls.
        world.tileMap.setTileSpriteLayer(mapX, mapY, 1, getRandomWestWall());
        world.tileMap.setTileSpriteLayer(mapX, (mapY + 1), 1,
                                         getRandomWestWall());
    }
    else {
        // No walls. First check if there are tiles to the north and west.
        if ((mapX > 0) && (mapY > 0)) {
            // There are tiles to the north and west. If they have walls
            // that form a gap, fill it.
            const Tile& northTile{world.tileMap.getTile(mapX, (mapY - 1))};
            const Tile& westTile{world.tileMap.getTile((mapX - 1), mapY)};
            if (hasWestWall(northTile) && hasNorthWall(westTile)) {
                world.tileMap.setTileSpriteLayer(mapX, mapY, 1,
                                                 getRandomNWFill());
            }
        }
    }
}

void MazeGenerationSystem::clearTilesTouchingEntity(MazeTopology& maze,
                                                    const entt::entity entity)
{
    // Get the tile extent that this entity is touching, clip it to the
    // maze bounds, and make it relative to the maze origin.
    const Collision& collision{world.registry.get<Collision>(entity)};
    TileExtent tileExtent{collision.worldBounds.asTileExtent()};
    tileExtent.intersectWith(mazeExtent);
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

    const TileExtent abstractTileExtent{topLeft.x, topLeft.y,
                                        (bottomRight.x - topLeft.x),
                                        (bottomRight.y - topLeft.y)};

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

int MazeGenerationSystem::getRandomWestWall()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 70) {
        return westWallIDs[4];
    }
    else if (randValue >= 70 && randValue < 90) {
        return westWallIDs[5];
    }
    else if (randValue >= 90 && randValue < 93) {
        return westWallIDs[2];
    }
    else if (randValue >= 93 && randValue < 96) {
        return westWallIDs[3];
    }
    else if (randValue >= 96 && randValue < 98) {
        return westWallIDs[0];
    }
    else {
        return westWallIDs[1];
    }
}

int MazeGenerationSystem::getRandomNorthWall()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 70) {
        return northWallIDs[4];
    }
    else if (randValue >= 70 && randValue < 90) {
        return northWallIDs[5];
    }
    else if (randValue >= 90 && randValue < 93) {
        return northWallIDs[2];
    }
    else if (randValue >= 93 && randValue < 96) {
        return northWallIDs[3];
    }
    else if (randValue >= 96 && randValue < 98) {
        return northWallIDs[0];
    }
    else {
        return northWallIDs[1];
    }
}

int MazeGenerationSystem::getRandomNEFill()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 80) {
        return northeastFillIDs[0];
    }
    else if (randValue >= 80 && randValue < 90) {
        return northeastFillIDs[1];
    }
    else {
        return northeastFillIDs[2];
    }
}

int MazeGenerationSystem::getRandomNWFill()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 80) {
        return northwestFillIDs[0];
    }
    else if (randValue >= 80 && randValue < 90) {
        return northwestFillIDs[1];
    }
    else {
        return northwestFillIDs[2];
    }
}

int MazeGenerationSystem::getRandomFullFill()
{
    std::uniform_int_distribution<int> dist{0, 99};
    int randValue{dist(randGenerator)};
    if (randValue < 60) {
        return fullFillIDs[0];
    }
    else if (randValue >= 60 && randValue < 80) {
        return fullFillIDs[1];
    }
    else {
        return fullFillIDs[2];
    }
}

bool MazeGenerationSystem::hasWestWall(const Tile& tile)
{
    if (tile.spriteLayers.size() > 1) {
        for (int spriteID : westWallIDs) {
            if (tile.spriteLayers[1].sprite.numericID == spriteID) {
                return true;
            }
        }
    }

    return false;
}

bool MazeGenerationSystem::hasNorthWall(const Tile& tile)
{
    if (tile.spriteLayers.size() > 1) {
        for (int spriteID : northWallIDs) {
            if (tile.spriteLayers[1].sprite.numericID == spriteID) {
                return true;
            }
        }
    }

    return false;
}

} // End namespace Server
} // End namespace AM
