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
struct ItemDescription
{
    /** The max length of an item's description text. */
    static constexpr std::size_t MAX_TEXT_LENGTH{500};

    std::string text{};
};
template<typename S>
void serialize(S& serializer, ItemDescription& itemDescription)
{
    serializer.text1b(itemDescription.text, ItemDescription::MAX_TEXT_LENGTH);
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
using ItemProperty = std::variant<ItemDescription, SeedType>;

} // End namespace AM
