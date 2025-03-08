#pragma once

#include "ItemInteractionType.h"
#include "EntityInteractionType.h"
#include "SpellType.h"
#include <functional>

namespace AM
{

struct ItemInteraction;
struct EntityInteraction;
struct Spell;

/**
 * Use this class to define all of the castables in your project (item 
 * interactions, entity interactions, and spells).
 */
struct CastableInit {
public:
    static void initItemInteractions(
        std::function<void(ItemInteractionType, const ItemInteraction&)>
            addItemInteraction);

    static void initEntityInteractions(
        std::function<void(EntityInteractionType, const EntityInteraction&)>
            addEntityInteraction);

    static void
        initSpells(std::function<void(SpellType, const Spell&)> addSpell);
};

} // namespace AM
