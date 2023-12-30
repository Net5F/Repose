#pragma once

#include <SDL_stdinc.h>

namespace AM
{
namespace Client
{
/** The types of build modes that we support. */
struct BuildMode {
    enum Type : Uint8 {
        Floor,
        FloorCovering,
        Wall,
        Object,
        Entity,
        Remove,
        Item,
        Count,
        None
    };
};

} // End namespace Client
} // End namespace AM
