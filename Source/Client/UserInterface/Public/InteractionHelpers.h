#pragma once

#include "EngineInteractionType.h"
#include "ProjectInteractionType.h"
#include "Log.h"
#include <SDL_stdinc.h>
#include <string>

namespace AM
{
namespace Client
{

/**
 * Shared static functions for dealing with interactions.
 */
class InteractionHelpers
{
public:
    static std::string toDisplayString(Uint8 interactionType)
    {
        switch (interactionType) {
            // Engine interaction types.
            case InteractionType::Temp:
                return "Temp";
            // Project interaction types.
            case InteractionType::Replant:
                return "Replant";
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }
};

} // namespace Client
} // namespace AM
