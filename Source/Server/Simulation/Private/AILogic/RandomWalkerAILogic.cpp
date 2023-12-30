#include "RandomWalkerAILogic.h"
#include "World.h"
#include "Input.h"
#include "PreviousPosition.h"

namespace AM
{
namespace Server
{

RandomWalkerAILogic::RandomWalkerAILogic(World& inWorld, entt::entity inEntity,
                                         double inTimeToWalk,
                                         double inTimeToWait,
                                         double inTimeTillDirectionChange)
: AILogic(inWorld, inEntity)
, timeToWalk{inTimeToWalk}
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

void RandomWalkerAILogic::tick()
{
    // If it's time to start or stop walking, do it.
    if ((shouldWalk && (walkTimer.getTime() > timeToWalk))
        || (!shouldWalk && (walkTimer.getTime() > timeToWait))) {
        shouldWalk = !shouldWalk;
        updateInputs();
        walkTimer.reset();
    }

    // If it's time to change directions, generate a new random direction.
    if (directionTimer.getTime() > timeTillDirectionChange) {
        currentInputIndex = inputDistribution(generator);
        updateInputs();
        directionTimer.reset();
    }
}

void RandomWalkerAILogic::updateInputs()
{
    world.registry.patch<Input>(entity, [&](auto& input) {
        // Release all of the inputs.
        for (Input::State& state : input.inputStates) {
            state = Input::State::Released;
        }

        // If we should be walking, press the input at the current index.
        if (shouldWalk) {
            input.inputStates[currentInputIndex] = Input::State::Pressed;
        }
    });
}

} // namespace Server
} // namespace AM
