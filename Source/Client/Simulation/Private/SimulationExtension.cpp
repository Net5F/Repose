#include "SimulationExtension.h"
#include "SimulationExDependencies.h"
#include "ComponentTypeRegistry.h"
#include "ReplicatedComponentTypes.h"
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
    // Register our component types.
    ComponentTypeRegistry& componentTypeRegistry{
        deps.simulation.getComponentTypeRegistry()};
    componentTypeRegistry
        .registerReplicatedComponents<ReplicatedComponentTypes>();
}

SimulationExtension::~SimulationExtension() = default;

void SimulationExtension::initializeSystems()
{
    animationSystem = std::make_unique<AnimationSystem>(
        deps.simulation.getWorld(), deps.spriteData);
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
