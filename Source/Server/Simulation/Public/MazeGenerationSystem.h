#pragma once

#include "Timer.h"
#include "Tile.h"
#include "TileExtent.h"
#include "TilePosition.h"
#include "entt/fwd.hpp"
#include <SDL_rect.h>
#include <random>
#include <array>

namespace AM
{
namespace Server
{

class World;
class SpriteData;
struct MazeTopology;
struct MazeCell;

/**
 * Generates the maze in the middle of the world. Every so often, the maze is
 * regenerated.
 *
 * The regenerated maze will line up with the pre-set exits, and will make sure
 * that players within the maze have valid paths to the exits.
 */
class MazeGenerationSystem
{
public:
    MazeGenerationSystem(World& inWorld, SpriteData& inSpriteData);

    /**
     * If MAZE_REGENERATION_PERIOD_S seconds have passed, regenerates the maze.
     */
    void regenerateMazeIfNecessary();

private:
    /**
     * Returns the index in the MazeTopology's cells vector where the cell with
     * the given coordinates can be found.
     */
    inline std::size_t linearizeCellIndex(int x, int y) const
    {
        return (y * abstractMazeExtent.xLength) + x;
    }

    /** How often the maze should be regenerated. */
    static constexpr float MAZE_REGENERATION_PERIOD_S{60 * 5};

    /** The top left tile of the maze. */
    static constexpr SDL_Point MAZE_ORIGIN_TILE{16, 18};

    /** The width of the maze, in tiles. */
    static constexpr unsigned int MAZE_WIDTH{36};

    /** The height of the maze, in tiles. */
    static constexpr unsigned int MAZE_HEIGHT{36};

    /**
     * Generates a new maze topology.
     *
     * @post outMaze holds a newly generated maze topology.
     */
    void generateMaze(MazeTopology& outMaze);

    /**
     * Clears a path from the given starting position to an exit.
     */
    void clearToExit(MazeTopology& maze, const TilePosition& startPosition,
                     int passNumber);

    /**
     * Clears a path from the given starting position to an already-visited tile
     * or an exit.
     */
    void clearToVisitedOrExit(MazeTopology& maze,
                              const TilePosition& startPosition,
                              int passNumber);

    void clearFromPlayersToExit();

    /**
     * Fills outNeighbors with the given position's neighbors.
     *
     * @param includeVisited  If false, neighbors will be excluded if
     *                        neighborCell.wasVisited == true.
     * @post outNeighbors now holds the valid neighbors, if any were found.
     */
    void getNeighboringTiles(const MazeTopology& maze,
                             const std::vector<TilePosition>& path,
                             bool includeVisited, int passNumber,
                             std::vector<TilePosition>& outNeighbors);

    /**
     * Returns true if the given position is in the exitTiles vector.
     */
    bool isExitTile(const TilePosition& position);

    /**
     * Chooses a random neighbor from the given vector, clears the appropriate
     * wall, and pushes the neighbor into the path.
     *
     * @post The maze topology has been updated appropriately, and path.back()
     *       is now set to the chosen neighbor.
     */
    void clearAndMoveToRandomNeighbor(
        MazeTopology& maze, const std::vector<TilePosition>& neighbors,
        std::vector<TilePosition>& path, int passNumber);

    /**
     * Checks all neighbors in the given vector. If any have already been
     * visited, clears the appropriate wall.
     *
     * @return true if a visited neighbor was found, else false.
     */
    bool clearToNeighborIfVisited(MazeTopology& maze,
                                  const std::vector<TilePosition>& neighbors,
                                  std::vector<TilePosition>& path,
                                  int passNumber);

    /**
     * Finds any cell that is enclosed by 4 walls and flags it to use the
     * "full fill" sprite.
     */
    void fillEnclosedCells(MazeTopology& maze);

    /**
     * Sets the map to reflect the given maze topology.
     */
    void applyMazeToMap(const MazeTopology& maze);

    /**
     * Applies the given cell to the 2x2 map area starting at the given
     * coordinates.
     */
    void applyCellToMap(int mapX, int mapY, const MazeCell& cell);

    /**
     * Clears the walls in all tiles that are touching the given entity.
     */
    void clearTilesTouchingEntity(MazeTopology& maze,
                                  const entt::entity entity);

    /**
     * These functions return a (weighted) random sprite ID from the chosen
     * category.
     */
    int getRandomWestWall();
    int getRandomNorthWall();
    int getRandomNEFill();
    int getRandomNWFill();
    int getRandomFullFill();

    /** Used to get entity positions and modify the tile map. */
    World& world;

    /** Used to get the wall sprites that we apply to the tile map. */
    SpriteData& spriteData;

    /** Used to track how much time has passed since the last maze
        regeneration. */
    Timer regenerationTimer;

    /** The maze's total extent in world-space. */
    const TileExtent mazeExtent;

    /** The extent of our abstract representation of the maze that we use
        during generation.
        Since we want the hallways to be 2 tiles wide, we make the abstract
        representation half the size of the real thing. Then at the end, we
        double it. */
    const TileExtent abstractMazeExtent;

    /** The abstract-space coordinates of the maze's entrance. */
    TilePosition entranceTile;

    /** A list of the abstract-space coordinates of each exit tile. */
    std::vector<TilePosition> exitTiles;

    /** Used to store the tiles that we've traversed during generation. */
    std::vector<TilePosition> workingPath;

    /** Used to store neighboring tiles during generation. */
    std::vector<TilePosition> workingNeighbors;

    /** Used for randomly choosing tiles. */
    std::mt19937 randGenerator;

    /** Used for placing randomized sprites in the map. */
    std::array<int, 6> northWallIDs;
    std::array<int, 6> westWallIDs;
    std::array<int, 3> northeastFillIDs;
    std::array<int, 3> northwestFillIDs;
    std::array<int, 3> fullFillIDs;
};

} // End namespace Server
} // End namespace AM
