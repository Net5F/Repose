#pragma once

#include "ISimulationExtension.h"
#include "SimulationExDependencies.h"
#include "BuildModeEntityToolSystem.h"
#include "MazeGenerationSystem.h"
#include "PlantSystem.h"
#include "TeleportSystem.h"

namespace AM
{
struct TileUpdateRequest;

namespace Server
{

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
     * Called after all entity movement state has been sent to the clients.
     */
    void afterMovementSync();

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
    /**
     * Called by the engine's TileUpdateSystem when a tile update request is 
     * received, before applying the update.
     * Allows the project to place constraints on map modifications, such as
     * requiring certain permissions, or only allowing updates to certain areas.
     *
     * @return true if the extent is editable, else false.
     */
    bool isExtentEditable(const TileExtent& tileExtent) override;

private:
    BuildModeEntityToolSystem buildModeEntityToolSystem;
    MazeGenerationSystem mazeGenerationSystem;
    PlantSystem plantSystem;
    TeleportSystem teleportSystem;
};

} // End namespace Server
} // End namespace AM
