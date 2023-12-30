#pragma once

#include "AILogic.h"
#include "Timer.h"
#include "entt/fwd.hpp"
#include <string>
#include <random>

namespace AM
{
namespace Server
{
class World;

/**
 * Makes the entity walk around randomly.
 */
class RandomWalkerAILogic : public AILogic
{
public:
    /**
     * @param inTimeToWalk How long to walk for.
     * @param inTimeToWait How long to wait for.
     * @param inTimeTillDirectionChange How often to change direction.
     */
    RandomWalkerAILogic(World& inWorld, entt::entity inEntity,
                        double inTimeToWalk, double inTimeToWait,
                        double inTimeTillDirectionChange);

    /**
     * Processes one iteration of AI logic.
     *
     * If the given entity doesn't possess any of the necessary components,
     * prints a warning and returns early.
     *
     * @param entity The entity that this AI is controlling.
     */
    void tick() override;

private:
    /**
     * Updates the entity's Input::inputStates to match the current AI state.
     */
    void updateInputs();

    /** How long to walk for. */
    double timeToWalk;
    /** How long to wait for. */
    double timeToWait;
    /** How often to change direction. */
    double timeTillDirectionChange;

    /** If true, our entity should be walking. */
    bool shouldWalk;

    /** The index of the currently active input in the entity's Input::
        inputStates. */
    int currentInputIndex;

    Timer walkTimer;
    Timer directionTimer;

    std::random_device randomDevice;
    std::mt19937 generator;
    std::uniform_int_distribution<> inputDistribution;
};

} // namespace Server
} // namespace AM
