#pragma once

#include <SDL_stdinc.h>
#include <vector>

namespace AM
{

    // TODO: Would it be useful to move this out to its own header?
/**
 * The types of interactions that a user may be able to perform on an entity.
 */
enum class InteractionType : Uint8 {
    NotSet,
    Use
};

// TODO: Do we want this, or a map?
std::string getInteractionDisplayName(InteractionType type) {
    if (type == InteractionType::Use) {
        return "Use";
    }
}

/**
 * Represents the interactions that an entity supports, i.e. the options that 
 * show up when you right click them.
 */
struct Interactions {
    /** The interactions that this entity supports. These show up in the 
        right-click list. */
    std::vector<InteractionType> supportedInteractions{};

    /** This entity's default interaction, i.e. the left-click interaction. */
    InteractionType defaultInteraction{InteractionType::NotSet};
};

} // End namespace AM
