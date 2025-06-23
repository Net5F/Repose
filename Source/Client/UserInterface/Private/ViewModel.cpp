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

entt::entity ViewModel::getHoveredEntity() const
{
    return hoveredEntity;
}

void ViewModel::setTargetEntity(entt::entity entity)
{
    if (entity != targetEntity) {
        targetEntity = entity;
        entityTargetedSig.publish(targetEntity);
    }
}
entt::entity ViewModel::getTargetEntity() const
{
    return targetEntity;
}

} // End namespace Client
} // End namespace AM
