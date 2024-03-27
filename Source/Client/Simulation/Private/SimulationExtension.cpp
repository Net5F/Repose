#include "SimulationExtension.h"
#include "SimulationExDependencies.h"
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

void SimulationExtension::initializeSystems() {}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates() {}

void SimulationExtension::afterSimUpdate() {}

void SimulationExtension::afterAll() {}

bool SimulationExtension::handleOSEvent(SDL_Event&)
{
    return false;
}

EntityGraphicType
    SimulationExtension::getUpdatedGraphicType(entt::entity)
{
    return EntityGraphicType::NotSet;
}

} // End namespace Client
} // End namespace AM
