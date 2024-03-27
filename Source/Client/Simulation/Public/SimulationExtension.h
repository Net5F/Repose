#pragma once

#include "ISimulationExtension.h"
#include "SimulationExDependencies.h"
#include <memory>

namespace AM
{
namespace Client
{
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
    void initializeSystems() override;

    /**
     * Called first in the simulation loop.
     */
    void beforeAll() override;

    /**
     * Called after the tile map is updated and peer entities are added/removed.
     */
    void afterMapAndConnectionUpdates() override;

    /**
     * Called after the general simulation update logic (movement, interactions,
     * etc.)
     */
    void afterSimUpdate() override;

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

    /**
     * Gives the project a chance to update the type of graphic that the given 
     * entity should use, based on its current sim state.
     *
     * The project must handle graphic type updates when the entity is doing 
     * something that the engine isn't aware of. For example, if the project 
     * implements swimming, this function should check if the entity is swimming
     * and return a Swimming graphic type if so.
     * 
     * If this function returns NotSet, the engine will set the entity's graphic 
     * type to one of the types in EngineEntityGraphicType.
     *
     * @return The entity's new graphic type, or NotSet if no project-implemented
     *         graphic types are appropriate.
     */
    EntityGraphicType getUpdatedGraphicType(entt::entity entity) override;

private:
    SimulationExDependencies deps;

    // Note: These are pointers so that we can delete/reconstruct them when we
    //       connect to the server. This gives them a consistent starting state.
    // std::unique_ptr<MySystem> mySystem;
};

} // End namespace Client
} // End namespace AM
