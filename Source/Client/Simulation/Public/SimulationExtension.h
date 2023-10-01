#pragma once

#include "ISimulationExtension.h"
#include <memory>

namespace AM
{
namespace Client
{
struct SimulationExDependencies;
class AnimationSystem;

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
    SimulationExtension(const SimulationExDependencies& inDeps);

    ~SimulationExtension();

    /**
     * Initializes or re-initializes our simulation systems.
     *
     * Used to put the systems in a consistent state, so they don't need to 
     * account for disconnects/reconnects.
     */
    void initializeSystems();

    /**
     * Called first in the simulation loop.
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
    SimulationExDependencies deps;

    // Note: These are pointers so that we can delete/reconstruct them when we 
    //       connect to the server. This gives them a consistent starting state.
    std::unique_ptr<AnimationSystem> animationSystem;
};

} // End namespace Client
} // End namespace AM
