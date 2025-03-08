#include "CastableInit.h"

namespace AM
{

void CastableInit::initItemInteractions(
    std::function<void(ItemInteractionType, const ItemInteraction&)>
        addItemInteraction)
{
}

void CastableInit::initEntityInteractions(
    std::function<void(EntityInteractionType, const EntityInteraction&)>
        addEntityInteraction)
{
}

void CastableInit::initSpells(
    std::function<void(SpellType, const Spell&)> addSpell)
{
}

} // namespace AM
