#include "RandomWalkerAI.h"
#include "World.h"
#include "Input.h"
#include "PreviousPosition.h"

namespace AM
{
namespace Server
{

RandomWalkerAI::RandomWalkerAI()
: timeToWalk{1}
, timeToWait{1}
, timeTillDirectionChange{1}
, shouldWalk{false}
, currentInputIndex{0}
, walkTimer{}
, directionTimer{}
, randomDevice{}
, generator{randomDevice()}
, inputDistribution{0, Input::YDown}
{
}

RandomWalkerAI::RandomWalkerAI(const RandomWalkerAI& other)
: timeToWalk{other.timeToWalk}
, timeToWait{other.timeToWait}
, timeTillDirectionChange{other.timeTillDirectionChange}
, shouldWalk{other.shouldWalk}
, currentInputIndex{other.currentInputIndex}
, walkTimer{}
, directionTimer{}
, randomDevice{}
, generator{randomDevice()}
, inputDistribution{0, Input::YDown}
{
}

RandomWalkerAI::RandomWalkerAI(double inTimeToWalk, double inTimeToWait,
                               double inTimeTillDirectionChange)
: timeToWalk{inTimeToWalk}
, timeToWait{inTimeToWait}
, timeTillDirectionChange{inTimeTillDirectionChange}
, shouldWalk{false}
, currentInputIndex{0}
, walkTimer{}
, directionTimer{}
, randomDevice{}
, generator{randomDevice()}
, inputDistribution{0, Input::YDown}
{
}

void RandomWalkerAI::tick(World& world, entt::entity entity)
{
    // If it's time to start or stop walking, do it.
    if ((shouldWalk && (walkTimer.getTime() > timeToWalk))
        || (!shouldWalk && (walkTimer.getTime() > timeToWait))) {
        shouldWalk = !shouldWalk;
        updateInputs(world, entity);
        walkTimer.reset();
    }

    // If it's time to change directions, generate a new random direction.
    if (directionTimer.getTime() > timeTillDirectionChange) {
        currentInputIndex = inputDistribution(generator);
        updateInputs(world, entity);
        directionTimer.reset();
    }
}

RandomWalkerAI& RandomWalkerAI::operator=(const RandomWalkerAI& other)
{
    timeToWalk = other.timeToWalk;
    timeToWait = other.timeToWait;
    timeTillDirectionChange = other.timeTillDirectionChange;
    shouldWalk = other.shouldWalk;
    currentInputIndex = other.currentInputIndex;
    walkTimer.reset();
    directionTimer.reset();

    return *this;
}

void RandomWalkerAI::updateInputs(World& world, entt::entity entity)
{
    world.registry.patch<Input>(entity, [&](Input& input) {
        // Release all of the inputs.
        input.inputStates.reset();

        // If we should be walking, press the input at the current index.
        if (shouldWalk) {
            input.inputStates[currentInputIndex] = Input::State::Pressed;
        }
    });
}

} // namespace Server
} // namespace AM
