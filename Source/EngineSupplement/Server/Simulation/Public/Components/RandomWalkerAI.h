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
 * AI behavior to make the entity walk around randomly.
 */
class RandomWalkerAI : public AILogic
{
public:
    RandomWalkerAI();

    RandomWalkerAI(const RandomWalkerAI& other);

    /**
     * @param inTimeToWalk How long to walk for.
     * @param inTimeToWait How long to wait for.
     * @param inTimeTillDirectionChange How often to change direction.
     */
    RandomWalkerAI(double inTimeToWalk, double inTimeToWait,
                   double inTimeTillDirectionChange);

    /**
     * Processes one iteration of AI logic.
     *
     * If the given entity doesn't possess any of the necessary components,
     * prints a warning and returns early.
     *
     * @param entity The entity that this AI is controlling.
     */
    void tick(World& world, entt::entity inEntity);

    RandomWalkerAI& operator=(const RandomWalkerAI& other);

    /** How long to walk for. */
    double timeToWalk;
    /** How long to wait for. */
    double timeToWait;
    /** How often to change direction. */
    double timeTillDirectionChange;

private:
    /**
     * Updates the entity's Input::inputStates to match the current AI state.
     */
    void updateInputs(World& world, entt::entity entity);

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

template<typename S>
void serialize(S& serializer, RandomWalkerAI& randomWalkerAI)
{
    // Note: We only serialize the configuration variables. 
    //       Current state variables will be defaulted.
    serializer.value8b(randomWalkerAI.timeToWalk);
    serializer.value8b(randomWalkerAI.timeToWait);
    serializer.value8b(randomWalkerAI.timeTillDirectionChange);
}

} // namespace Server
} // namespace AM
