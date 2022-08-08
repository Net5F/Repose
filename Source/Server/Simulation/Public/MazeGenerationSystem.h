#pragma once

#include "Timer.h"

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
    static constexpr float MAZE_REGENERATION_PERIOD_S{60 * 5};

    // TODO: Put origin and size and other consts here

    /**
     * Clears the maze area and generates a new maze topology.
     */
    void generateMaze();

    /** Used to get entity positions and modify the tile map. */
    World& world;

    /** Used to track how much time has passed since the last maze
        regeneration. */
    Timer regenerationTimer;
};

} // End namespace Server
} // End namespace AM
