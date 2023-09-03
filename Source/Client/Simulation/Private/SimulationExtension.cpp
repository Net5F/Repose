#include "SimulationExtension.h"
#include "Simulation.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Client
{

SimulationExtension::SimulationExtension(SimulationExDependencies deps)
: animationSystem{deps.simulation.getWorld(), deps.spriteData}
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates() {}

void SimulationExtension::afterMovement()
{
    animationSystem.updateAnimations();
}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Client
} // End namespace AM
