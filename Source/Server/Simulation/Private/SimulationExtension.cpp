#include "SimulationExtension.h"
#include "SimulationExDependencies.h"
#include "Simulation.h"
#include "Network.h"
#include "GraphicData.h"
#include "TileExtent.h"
#include "EntityInitRequest.h"
#include "EntityNameChangeRequest.h"
#include "GraphicStateChangeRequest.h"
#include "NoEdit.h"
#include "SystemMessage.h"
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
, teleportSystem{deps.simulation.getWorld()}
{
    // Add our Lua bindings.
    projectLuaBindings.addBindings();

    // Add an example item interaction handler.
    world.castHelper.setOnItemInteractionCompleted(
        ItemInteractionType::Test, [&](const CastInfo& castInfo) {
            deps.network.serializeAndSend(
                castInfo.clientID,
                SystemMessage{"Test interaction successful."});
        });

    // Add an example spell handler.
    // Note: For better organization, you'd normally define these in a 
    //       different file. This is just a quick example.
    world.castHelper.setOnSpellCompleted(SpellType::Fireball,
                                         [&](const CastInfo&) {
                                             // Here is where you'd subtract HP
                                             // from the target, or whatever you
                                             // want to do.
                                         });
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates()
{
    // Respond to any build mode data messages that aren't handled
    // by the engine.
    buildModeDataSystem.processMessages();
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
        // Only return true for updates within a build area.
        return isInBuildArea(tileExtent);
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
        // Only return true if the entity is within a build area and the 
        // request isn't trying to re-init one of our protected entities.
        // Note: We need to check entity != null because it's valid to have an 
        //       init request with a null entity ID (to create a new entity).
        entt::entity entity{entityInitRequest.entity};
        bool isEditable{true};
        if (entity != entt::null) {
            isEditable
                = !(world.registry.all_of<NoEdit>(entityInitRequest.entity));
        }
        return isInBuildArea(entityInitRequest.position) && isEditable;
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
        // Only return true if the entity is within a build area and isn't 
        // one of our protected entities.
        bool isEditable{!(world.registry.all_of<NoEdit>(entity))};
        const auto& position{world.registry.get<Position>(entity)};
        return isInBuildArea(position) && isEditable;
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
        // Only return true if the entity is within a build area and isn't 
        // one of our protected entities.
        bool isEditable{!(world.registry.all_of<NoEdit>(entity))};
        const auto& position{world.registry.get<Position>(entity)};
        return isInBuildArea(position) && isEditable;
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
        // Only return true if the entity is within a build area and isn't 
        // one of our protected entities.
        bool isEditable{!(world.registry.all_of<NoEdit>(entity))};
        const auto& position{world.registry.get<Position>(entity)};
        return isInBuildArea(position) && isEditable;
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isItemInitRequestValid(
    const ItemInitRequest& itemInitRequest) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Find the entity ID of the client that sent this request.
        auto it{world.netIDMap.find(itemInitRequest.netID)};
        if (it == world.netIDMap.end()) {
            // Client doesn't exist (may have disconnected), return false.
            return false;
        }
        entt::entity clientEntity{it->second};

        // Only return true if the entity is within a build area.
        const auto& position{world.registry.get<Position>(clientEntity)};
        return isInBuildArea(position);
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isItemChangeRequestValid(
    const ItemChangeRequest& itemChangeRequest) const
{
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        // Find the entity ID of the client that sent this request.
        auto it{world.netIDMap.find(itemChangeRequest.netID)};
        if (it == world.netIDMap.end()) {
            // Client doesn't exist (may have disconnected), return false.
            return false;
        }
        entt::entity clientEntity{it->second};

        // Only return true if the entity is within a build area and the item
        // isn't one of our protected items.
        bool isEditable{!(
            std::ranges::contains(PROTECTED_ITEMS, itemChangeRequest.itemID))};
        const auto& position{world.registry.get<Position>(clientEntity)};
        return isInBuildArea(position) && isEditable;
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

bool SimulationExtension::isInBuildArea(const TileExtent& tileExtent) const
{
    for (const TileExtent& buildAreaExtent : VALID_BUILD_AREA_EXTENTS) {
        if (buildAreaExtent.contains(tileExtent))
        {
            return true;
        }
    }

    return false;
}

bool SimulationExtension::isInBuildArea(const Position& position) const
{
    bool isInBuildArea{false};
    for (const TileExtent& buildAreaExtent : VALID_BUILD_AREA_EXTENTS) {
        if (buildAreaExtent.contains(TilePosition(position)))
        {
            return true;
        }
    }

    return false;
}

} // End namespace Server
} // End namespace AM
