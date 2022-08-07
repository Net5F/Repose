#include "MazeGenerationSystem.h"
#include "World.h"

namespace AM
{
namespace Server
{

MazeGenerationSystem::MazeGenerationSystem(World& inWorld)
: world{inWorld}
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
}

} // End namespace Server
} // End namespace AM
