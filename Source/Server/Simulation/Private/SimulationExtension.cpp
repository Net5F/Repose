#include "SimulationExtension.h"
#include "Simulation.h"
#include "Network.h"
#include "SpriteData.h"
#include "TileExtent.h"
#include "InteractionRequest.h"
#include "EntityInitRequest.h"
#include "NameChangeRequest.h"
#include "AnimationStateChangeRequest.h"
#include "BuildModeDefs.h"
#include "SharedConfig.h"
#include "Log.h"

namespace AM
{
namespace Server
{

SimulationExtension::SimulationExtension(SimulationExDependencies deps)
: world{deps.simulation.getWorld()}
, buildModeDataSystem{deps.simulation.getWorld(),
                      deps.network.getEventDispatcher(), deps.network,
                      deps.spriteData}
, mazeGenerationSystem{deps.simulation.getWorld(), deps.spriteData}
, plantSystem{deps.simulation, deps.spriteData}
, teleportSystem{deps.simulation.getWorld()}
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates()
{
    // Respond to any build mode data messages that aren't handled 
    // by the engine.
    buildModeDataSystem.processMessages();

    // Regenerate the maze, if enough time has passed.
    mazeGenerationSystem.regenerateMazeIfNecessary();

    // Update any plants that are ready for growth or replanting.
    plantSystem.updatePlants();
}

void SimulationExtension::afterMovement()
{
    // Teleport any players that are touching a teleport volume.
    teleportSystem.teleportPlayers();
}

void SimulationExtension::afterClientSync() {}

void SimulationExtension::afterAll() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

bool SimulationExtension::isTileExtentEditable(
    NetworkID, const TileExtent& tileExtent) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true for updates within the build area.
        return VALID_BUILD_AREA_EXTENT.containsExtent(tileExtent);
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isEntityInitRequestValid(
    const EntityInitRequest& entityInitRequest) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the new entity is within the build area.
        return VALID_BUILD_AREA_EXTENT.containsPosition(
            entityInitRequest.position.asTilePosition());
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isNameChangeRequestValid(
    const NameChangeRequest& nameChangeRequest) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the entity exists and is in the build area.
        entt::entity entity{nameChangeRequest.entity};
        if (world.entityIDIsInUse(entity)) {
            if (const auto* position
                = world.registry.try_get<Position>(entity)) {
                return VALID_BUILD_AREA_EXTENT.containsPosition(
                    position->asTilePosition());
            }
        }

        return false;
    }
    else {
        // No restrictions, return true if the entity exists.
        return world.entityIDIsInUse(nameChangeRequest.entity);
    }
}

bool SimulationExtension::isAnimationStateChangeRequestValid(
    const AnimationStateChangeRequest& animationStateChangeRequest) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the entity exists and is in the build area.
        entt::entity entity{animationStateChangeRequest.entity};
        if (world.entityIDIsInUse(entity)) {
            if (const auto* position
                = world.registry.try_get<Position>(entity)) {
                return VALID_BUILD_AREA_EXTENT.containsPosition(
                    position->asTilePosition());
            }
        }

        return false;
    }
    else {
        // No restrictions, return true if the entity exists.
        return world.entityIDIsInUse(animationStateChangeRequest.entity);
    }
}

} // End namespace Server
} // End namespace AM
