#pragma once

#include "CastableID.h"
#include "Castable.h"
#include <functional>

namespace AM
{

/**
 * Use this class to define all of the castables in your project (item 
 * interactions, entity interactions, and spells).
 */
class CastableDef {
public:
    static void defineCastables(
        std::function<void(CastableID, const Castable&)> addCastable)
    {
    }
};

} // namespace AM
