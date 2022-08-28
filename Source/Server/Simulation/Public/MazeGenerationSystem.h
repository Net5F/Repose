#pragma once

#include "Timer.h"
#include "TileExtent.h"
#include "TilePosition.h"
#include <SDL_rect.h>

namespace AM
{
namespace Server
{

class World;
class SpriteData;
struct MazeTopology;

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
     * Returns the index in the cells vector where the cells with the given
     * coordinates can be found.
     */
    inline unsigned int linearizeCellIndex(int x, int y) const
    {
        return (y * mazeExtent.xLength) + x;
    }

    /** How often the maze should be regenerated. */
    //static constexpr float MAZE_REGENERATION_PERIOD_S{60 * 5};
    static constexpr float MAZE_REGENERATION_PERIOD_S{30};

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
     * Sets the map to reflect the given maze topology.
     */
    void applyMazeToMap(const MazeTopology& maze);

    /**
     * Clears a path from the given start position, stopping when an exit is 
     * found.
     */
    void clearToExit(MazeTopology& maze, const TilePosition& startPosition);

    /**
     * Clears a path from the given start position, stopping when an already-
     * visited tile is found.
     */
    void clearToVisited(MazeTopology& maze, const TilePosition& startPosition);

    /**
     * Fills outNeighbors with the given position's neighbors.
     * 
     * @param includeVisited  If false, neighbors will be excluded if 
     *                        neighborCell.wasVisited == true.
     * @post outNeighbors now holds the valid neighbors, if any were found.
     */
    void getNeighboringTiles(const MazeTopology& maze,
                             const TilePosition& position, bool includeVisited,
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
    void
        clearAndMoveToRandomNeighbor(MazeTopology& maze,
                                     const std::vector<TilePosition>& neighbors,
                                     std::vector<TilePosition>& path);

    /** Used to get entity positions and modify the tile map. */
    World& world;

    /** Used to get the wall sprites that we apply to the tile map. */
    SpriteData& spriteData;

    /** Used to track how much time has passed since the last maze
        regeneration. */
    Timer regenerationTimer;

    /** The maze's total extent. */
    const TileExtent mazeExtent;

    /** The coordinates of the maze's entrance. */
    TilePosition entranceTile;

    /** A list of the coordinates of each exit tile.
        Note: These are in half-coordinates to match MazeTopology during 
              the generation process. */
    std::vector<TilePosition> exitTiles;

    /** Used to store the tiles that we've traversed during generation. */
    std::vector<TilePosition> workingPath;

    /** Used to store neighboring tiles during generation. */
    std::vector<TilePosition> workingNeighbors;

    // For randomly choosing tiles.
    std::random_device randDevice;
    std::mt19937 randGenerator;
};

} // End namespace Server
} // End namespace AM
