#pragma once

#include "ItemInteractionType.h"
#include "AVSequence.h"
#include "Log.h"
#include <string>

namespace AM
{

namespace DisplayStrings
{
/**
 * The string to display for each interaction.
 */
static std::string get(ItemInteractionType interactionType)
{
    switch (interactionType) {
        // Engine interaction types.
        case ItemInteractionType::UseOn:
            return "Use";
        case ItemInteractionType::Destroy:
            return "Destroy";
        case ItemInteractionType::Examine:
            return "Examine";
        // Project interaction types.
        case ItemInteractionType::TestAVEffect:
            return "Test";
        default:
            break;
    }

    LOG_ERROR("Tried to get display name for unknown interaction type.");
    return "?";
}
} // namespace DisplayStrings

namespace AVSequences
{
namespace detail
{
AVSequence testAVEffect() {
    AVSequence sequence{};

    // TODO: Do we need to pass graphicData into all of these?
    //       Or maybe it's time to make it global?
    // Get a numeric ID for the given string ID.
    // Note: If the string is invalid, this will be the null animation.
    //const Animation& animation{graphicData.getAnimation(animationID)};
    //AnimationID animationNumericID{animation.numericID};

    //sequence.addMovementPhase(AVSequence::FollowSelf, 10, 5);
    //sequence.addGraphic(toGraphicID(animationNumericID));

    return sequence;
}
} // namespace detail

/**
 * The audio/visual sequence to play when each interaction is triggered.
 */
static AVSequence get(ItemInteractionType interactionType) {
    using namespace detail;

    switch (interactionType) {
        // Engine interaction types.
        case ItemInteractionType::UseOn:
        case ItemInteractionType::Destroy:
        case ItemInteractionType::Examine:
            return {};
        // Project interaction types.
        case ItemInteractionType::TestAVEffect:
            return testAVEffect();
        default:
            break;
    }

    LOG_ERROR("Tried to get AVSequence for unknown interaction type.");
    return {};
}
} // namespace AVSequences

} // namespace AM
