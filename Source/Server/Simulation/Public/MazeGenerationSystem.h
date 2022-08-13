#pragma once

#include "Timer.h"
#include "TileExtent.h"
#include <SDL_rect.h>

namespace AM
{
namespace Server
{

class World;

/**
 * Generates the maze in the middle of the world. Every so often, the maze is
 * regenerated.
 *
 * When the maze is regenerated, it lines up to the pre-set exits and goes
 * around players.
 */
class MazeGenerationSystem
{
public:
    MazeGenerationSystem(World& inWorld);

    /**
     * If MAZE_REGENERATION_PERIOD_S seconds have passed, regenerates the maze.
     */
    void regenerateMazeIfNecessary();

private:
    /** How often the maze should be regenerated. */
    //static constexpr float MAZE_REGENERATION_PERIOD_S{60 * 5};
    static constexpr float MAZE_REGENERATION_PERIOD_S{30};

    /** The top left tile of the maze. */
    static constexpr SDL_Point MAZE_ORIGIN_TILE{16, 18};

    /** The length of a side of the maze, in tiles. */
    static constexpr unsigned int MAZE_SIDE_LENGTH{36};

    /**
     * Clears the maze area and generates a new maze topology.
     */
    void generateMaze();

    /** Used to get entity positions and modify the tile map. */
    World& world;

    /** Used to track how much time has passed since the last maze
        regeneration. */
    Timer regenerationTimer;

    /** The maze's total extent. */
    const TileExtent mazeExtent;
};

} // End namespace Server
} // End namespace AM
