#include "ViewModel.h"

namespace AM
{
namespace Client
{

ViewModel::ViewModel()
: hoveredEntity{entt::null}
, targetEntity{entt::null}
, entityHoveredSig{}
, entityTargetedSig{}
, entityHovered{entityHoveredSig}
, entityTargeted{entityTargetedSig}
{
}

void ViewModel::setHoveredEntity(entt::entity entity)
{
    if (entity != hoveredEntity) {
        hoveredEntity = entity;
        entityHoveredSig.publish(hoveredEntity);
    }
}

void ViewModel::setTargetEntity(entt::entity entity)
{
    if (entity != targetEntity) {
        targetEntity = entity;
        entityTargetedSig.publish(targetEntity);
    }
}

} // End namespace Client
} // End namespace AM
