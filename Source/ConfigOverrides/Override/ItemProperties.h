#pragma once

///////////////////////////////////////////////////////////////////////////////
// Note: This file overrides the engine's default 
//       SharedLib/ItemProperties.h
///////////////////////////////////////////////////////////////////////////////

#include <SDL_stdinc.h>
#include <string>
#include <variant>

namespace AM
{
/** The item's description. */
struct Description
{
    std::string text{};
};
template<typename S>
void serialize(S& serializer, Description& description)
{
    serializer.text1b(description.text);
}

/** The type of plant that will be grown. */
struct SeedType
{
    enum Value : Uint8 {
        Sunflower,
        Leek,
        Pepper
    };

    Value value{};
};
template<typename S>
void serialize(S& serializer, SeedType& seedType)
{
    serializer.value1b(seedType.value);
}

/** The list of properties that may be attached to an item. */
using ItemProperty = std::variant<Description, SeedType>;

} // End namespace AM
