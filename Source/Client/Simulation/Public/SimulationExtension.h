#pragma once

#include "ISimulationExtension.h"
#include "SimulationExDependencies.h"
#include "AnimationSystem.h"

namespace AM
{
namespace Client
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

    /**
     * Called before any systems are ran.
     */
    void beforeAll();

    /**
     * Called after the tile map is updated and peer entities are added/removed.
     */
    void afterMapAndConnectionUpdates();

    /**
     * Called after all entity movement has been processed.
     */
    void afterMovement();

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

private:
    AnimationSystem animationSystem;
};

} // End namespace Client
} // End namespace AM
