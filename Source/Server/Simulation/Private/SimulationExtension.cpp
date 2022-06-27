#include "SimulationExtension.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Server
{

SimulationExtension::SimulationExtension([[maybe_unused]] SimulationExDependencies deps)
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndLifetimeUpdates() {}

void SimulationExtension::afterMovement() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Server
} // End namespace AM
