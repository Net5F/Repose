#pragma once

#include "EntityInteractionType.h"
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
static std::string get(EntityInteractionType interactionType)
{
    switch (interactionType) {
        // Engine interaction types.
        case EntityInteractionType::Talk:
            return "Talk";
        // Project interaction types.
        case EntityInteractionType::Replant:
            return "Replant";
        default:
            LOG_ERROR(
                "Tried to get display name for unknown interaction type.");
            return "?";
    }

    return "";
}
} // namespace DisplayStrings

namespace AVSequences
{
namespace detail
{
AVSequence replant() {
    AVSequence sequence{};
    return sequence;
}
} // namespace detail

/**
 * The audio/visual sequence to play when each interaction is triggered.
 */
static AVSequence get(EntityInteractionType interactionType) {
    using namespace detail;

    switch (interactionType) {
        // Engine interaction types.
        case EntityInteractionType::Talk:
            return {};
        // Project interaction types.
        case EntityInteractionType::Replant:
            return replant();
        default:
            break;
    }

    LOG_ERROR("Tried to get AVSequence for unknown interaction type.");
    return {};
}
} // namespace AVSequences

} // namespace AM
