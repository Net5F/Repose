#include "SimulationExtension.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Server
{

SimulationExtension::SimulationExtension(SimulationExDependencies deps)
: mazeGenerationSystem{deps.world, deps.spriteData}
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates()
{
    // Regenerate the maze, if enough time has passed.
    mazeGenerationSystem.regenerateMazeIfNecessary();
}

void SimulationExtension::afterMovement() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Server
} // End namespace AM
