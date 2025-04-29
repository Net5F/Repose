#include "SimulationExtension.h"
#include "SimulationExDependencies.h"
#include "Simulation.h"
#include "Network.h"
#include "GraphicData.h"
#include "TileExtent.h"
#include "EntityInitRequest.h"
#include "EntityNameChangeRequest.h"
#include "GraphicStateChangeRequest.h"
#include "BuildModeDefs.h"
#include "SharedConfig.h"
#include "Log.h"

namespace AM
{
namespace Server
{

SimulationExtension::SimulationExtension(const SimulationExDependencies& deps)
: world{deps.simulation.getWorld()}
, projectLuaBindings{deps.simulation.getEntityInitLua(),
                     deps.simulation.getEntityItemHandlerLua(),
                     deps.simulation.getItemInitLua(),
                     deps.simulation.getDialogueLua(),
                     deps.simulation.getDialogueChoiceConditionLua(),
                     deps.graphicData,
                     world}
, buildModeDataSystem{world, deps.network.getEventDispatcher(), deps.network,
                      deps.graphicData}
, mazeGenerationSystem{world, deps.graphicData}
, plantSystem{world, deps.graphicData}
, teleportSystem{deps.simulation.getWorld()}
{
    // Add our Lua bindings.
    projectLuaBindings.addBindings();
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

void SimulationExtension::afterSimUpdate()
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
        return VALID_BUILD_AREA_EXTENT.contains(tileExtent);
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
        return VALID_BUILD_AREA_EXTENT.contains(
            TilePosition(entityInitRequest.position));
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isEntityDeleteRequestValid(
    const EntityDeleteRequest& entityDeleteRequest) const
{
    entt::entity entity{entityDeleteRequest.entity};

    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the entity is within the build area.
        const auto& position{world.registry.get<Position>(entity)};
        return VALID_BUILD_AREA_EXTENT.contains(TilePosition(position));

        return false;
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isEntityNameChangeRequestValid(
    const EntityNameChangeRequest& nameChangeRequest) const
{
    entt::entity entity{nameChangeRequest.entity};

    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the entity is within the build area.
        const auto& position{world.registry.get<Position>(entity)};
        return VALID_BUILD_AREA_EXTENT.contains(TilePosition(position));
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isGraphicStateChangeRequestValid(
    const GraphicStateChangeRequest& graphicStateChangeRequest) const
{
    entt::entity entity{graphicStateChangeRequest.entity};

    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Only return true if the entity is within the build area.
        const auto& position{world.registry.get<Position>(entity)};
        return VALID_BUILD_AREA_EXTENT.contains(TilePosition(position));
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

} // End namespace Server
} // End namespace AM
