#pragma once

#include "ISimulationExtension.h"
#include "ProjectLuaBindings.h"
#include "BuildModeDataSystem.h"
#include "MazeGenerationSystem.h"
#include "PlantSystem.h"
#include "TeleportSystem.h"

namespace AM
{
namespace Server
{
class World;
struct SimulationExDependencies;

/**
 * An extension of the engine's Simulation class.
 *
 * Manages the project-level simulation, including world state and system
 * processing.
 */
class SimulationExtension : public ISimulationExtension
{
public:
    // Note: This is the canonical constructor, expected by the factory that
    //       constructs this class. Do not modify it.
    SimulationExtension(const SimulationExDependencies& deps);

    //-------------------------------------------------------------------------
    // Simulation Tick Hooks (Call your systems in these)
    //-------------------------------------------------------------------------
    /**
     * Called before any systems are ran.
     */
    void beforeAll() override;

    /**
     * Called after the tile map is updated and client entities are
     * added/removed.
     */
    void afterMapAndConnectionUpdates() override;

    /**
     * Called after the general simulation update logic (movement, interactions,
     * etc.)
     */
    void afterSimUpdate() override;

    /**
     * Called after all relevant state has been sent to the clients.
     */
    void afterClientSync() override;

    /**
     * Called after all other systems.
     */
    void afterAll() override;

    /**
     * See OSEventHandler for details.
     *
     * Note: Simulation will pass events to this class first. If the event is
     *       not handled, then Simulation will attempt to handle it.
     */
    bool handleOSEvent(SDL_Event& event) override;

    //-------------------------------------------------------------------------
    // Simulation System Hooks (Hooks into engine systems)
    //-------------------------------------------------------------------------
    // These functions allow the project to affect various World state 
    // modifications.

    // TODO: Replace these with a permissions system.
    /** @return true if the given extent is editable, else false. */
    bool isTileExtentEditable(NetworkID netID,
                              const TileExtent& tileExtent) const override;
    /** @return true if the given request is valid, else false. */
    bool isEntityInitRequestValid(
        const EntityInitRequest& entityInitRequest) const override;
    bool isEntityDeleteRequestValid(
        const EntityDeleteRequest& entityDeleteRequest) const override;
    bool isEntityNameChangeRequestValid(
        const EntityNameChangeRequest& nameChangeRequest) const override;
    bool isGraphicStateChangeRequestValid(
        const GraphicStateChangeRequest& graphicStateChangeRequest)
        const override;

private:
    /** Used to validate change requests. */
    World& world;

    /** This project's Lua bindings. */
    ProjectLuaBindings projectLuaBindings;

    BuildModeDataSystem buildModeDataSystem;
    MazeGenerationSystem mazeGenerationSystem;
    PlantSystem plantSystem;
    TeleportSystem teleportSystem;
};

} // End namespace Server
} // End namespace AM
