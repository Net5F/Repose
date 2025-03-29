#pragma once

#include "CastableID.h"
#include <functional>

namespace AM
{

struct Castable;

/**
 * Use this class to define all of the castables in your project (item 
 * interactions, entity interactions, and spells).
 */
struct CastableInit {
public:
    static void initCastables(
        std::function<void(CastableID, const Castable&)> addCastable);
};

} // namespace AM
