#include "SimulationExtension.h"
#include "SimulationExDependencies.h"
#include "AnimationSystem.h"
#include "Simulation.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Client
{

SimulationExtension::SimulationExtension(const SimulationExDependencies& inDeps)
: deps{inDeps}
{
}

SimulationExtension::~SimulationExtension() = default;

void SimulationExtension::initializeSystems()
{
    animationSystem = std::make_unique<AnimationSystem>(
        deps.simulation.getWorld(), deps.graphicData);
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates() {}

void SimulationExtension::afterSimUpdate()
{
    animationSystem->updateAnimations();
}

void SimulationExtension::afterAll() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Client
} // End namespace AM
