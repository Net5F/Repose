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
        if (interactionType < EngineInteractionType::PROJECT_START) {
            EngineInteractionType::Value engineInteractionType{
                static_cast<EngineInteractionType::Value>(interactionType)};
            switch (engineInteractionType) {
                case EngineInteractionType::Temp:
                    return "Temp";
                default:
                    break;
            }
        }
        else {
            ProjectInteractionType::Value projectInteractionType{
                static_cast<ProjectInteractionType::Value>(interactionType)};
            switch (projectInteractionType) {
                case ProjectInteractionType::Replant:
                    return "Replant";
                default:
                    break;
            }
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }
};

} // namespace Client
} // namespace AM
