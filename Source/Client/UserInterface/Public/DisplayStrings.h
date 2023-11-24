#pragma once

#include "EntityInteractionType.h"
#include "ItemInteractionType.h"
#include "Log.h"
#include <SDL_stdinc.h>
#include <string>

namespace AM
{
namespace Client
{

/**
 * Shared static functions for converting enums to display strings.
 */
class DisplayStrings
{
public:
    static std::string get(EntityInteractionType interactionType)
    {
        switch (interactionType) {
            // Engine interaction types.
            // Project interaction types.
            case EntityInteractionType::Replant:
                return "Replant";
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }

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
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }
};

} // namespace Client
} // namespace AM
