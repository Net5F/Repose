#pragma once

#include "EngineMessageType.h"
#include <SDL_stdinc.h>

namespace AM
{
/**
 * The types of messages that we send across the network.
 *
 * This file extends SharedLib/Messages/EngineMessageType.h. That file holds 
 * engine-defined message types, this file holds project-defined message types.
 *
 * For message descriptions, see their definitions in Shared/Messages/Public.
 */
enum class ProjectMessageType : Uint8 {
    // Note: The engine reserves values 0 - 124. 125 - 255 are available.

    // Client -> Server Messages
    EntityTemplatesRequest
      = static_cast<Uint8>(EngineMessageType::PROJECT_START),
    AddEntityTemplate,
    TemplateInitScriptRequest,

    // Server -> Client Messages
    EntityTemplates,
    EntityInitScript,

    // Bidirectional Messages
};

} // End namespace AM
