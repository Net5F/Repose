#pragma once

#include "ISimulationExtension.h"
#include "SimulationExDependencies.h"
#include "BuildModeDataSystem.h"
#include "MazeGenerationSystem.h"
#include "PlantSystem.h"
#include "TeleportSystem.h"

namespace AM
{
namespace Server
{
class World;

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
    SimulationExtension(SimulationExDependencies deps);

    //-------------------------------------------------------------------------
    // Simulation Tick Hooks (Call your systems in these)
    //-------------------------------------------------------------------------
    /**
     * Called before any systems are ran.
     */
    void beforeAll();

    /**
     * Called after the tile map is updated and client entities are
     * added/removed.
     */
    void afterMapAndConnectionUpdates();

    /**
     * Called after all entity movement has been processed, but before the new
     * movement state has been sent to the clients.
     */
    void afterMovement();

    /**
     * Called after all relevant state has been sent to the clients.
     */
    void afterClientSync();

    /**
     * Called after all other systems.
     */
    void afterAll();

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
    // TODO: Replace this with a permissions system.
    // These functions allow the project to place constraints on various 
    // World state modifications.
    /** @return true if the given extent is editable, else false. */
    bool isTileExtentEditable(NetworkID netID,
                              const TileExtent& tileExtent) const;
    /** @return true if the given request is valid, else false. */
    bool isEntityInitRequestValid(
        const EntityInitRequest& entityInitRequest) const;
    bool isNameChangeRequestValid(
        const NameChangeRequest& nameChangeRequest) const;
    bool isAnimationStateChangeRequestValid(
        const AnimationStateChangeRequest& nameChangeRequest) const;

private:
    /** Used to validate change requests. */
    World& world;

    BuildModeDataSystem buildModeDataSystem;
    MazeGenerationSystem mazeGenerationSystem;
    PlantSystem plantSystem;
    TeleportSystem teleportSystem;
};

} // End namespace Server
} // End namespace AM
