#pragma once

#include "ISimulationExtension.h"
#include "SimulationExDependencies.h"

namespace AM
{
namespace Client
{

/**
 * PLACEHOLDER - Currently not simulating anything at the project level.
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
     * Called after the tile map is updated and NPCs are added/removed.
     */
    void afterMapAndLifetimeUpdates();

    /**
     * Called after all entity movement has been processed.
     */
    void afterMovement();

    /**
     * See OSEventHandler for details.
     *
     * Note: Simulation will pass events to this class first. If the event is
     *       not handled, then Simulation will attempt to handle it.
     */
    bool handleOSEvent(SDL_Event& event) override;

private:
};

} // End namespace Client
} // End namespace AM
