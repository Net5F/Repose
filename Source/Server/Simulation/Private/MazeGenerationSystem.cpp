#include "MazeGenerationSystem.h"
#include "World.h"

namespace AM
{
namespace Server
{

MazeGenerationSystem::MazeGenerationSystem(World& inWorld)
: world{inWorld}
, mazeExtent{MAZE_ORIGIN_TILE.x, MAZE_ORIGIN_TILE.y,
             MAZE_SIDE_LENGTH, MAZE_SIDE_LENGTH}
{
    // Generate the initial maze state.
    generateMaze();

    regenerationTimer.updateSavedTime();
}

void MazeGenerationSystem::regenerateMazeIfNecessary()
{
    // If enough time has passed, regenerate the maze.
    if (regenerationTimer.getDeltaSeconds(false)
        >= MAZE_REGENERATION_PERIOD_S) {
        generateMaze();

        regenerationTimer.updateSavedTime();
    }
}

void MazeGenerationSystem::generateMaze()
{
    // Clear the area.
    world.tileMap.clearExtent(mazeExtent, 1);
    LOG_INFO("Cleared area.");

    // Generate the new maze.

    // TODO: Make a function in world or tileMap, maybe with a queue handled by a 
    //       system, that takes tile updates, applies them, and ships messages to 
    //       anyone in range. Systems shouldn't have to know about message range.
    // TODO: Make TileUpdate have a vector?
    // Send the tile updates to all clients that are in range.
}

} // End namespace Server
} // End namespace AM
