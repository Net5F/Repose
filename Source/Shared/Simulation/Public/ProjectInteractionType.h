#pragma once

#include "EngineInteractionType.h"
#include <SDL_stdinc.h>

namespace AM
{
namespace InteractionType
{

/**
 * The types of interactions that a user may be able to perform on an entity.
 * 
 * This file extends SharedLib/Simulation/EngineInteractionType.h. That file 
 * holds engine-defined interaction types, this file holds project-defined 
 * interaction types.
 */
enum Project : Uint8
{
    // Note: The engine reserves values 0 - 124. 125 - 255 are available.

    // Sunflower garden
    Replant = static_cast<Uint8>(InteractionType::PROJECT_START),
};

} // End namespace InteractionType
} // End namespace AM
