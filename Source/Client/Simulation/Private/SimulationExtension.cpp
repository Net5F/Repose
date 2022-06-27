#include "SimulationExtension.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Client
{

SimulationExtension::SimulationExtension(
    [[maybe_unused]] SimulationExDependencies deps)
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndLifetimeUpdates() {}

void SimulationExtension::afterMovement() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Client
} // End namespace AM
