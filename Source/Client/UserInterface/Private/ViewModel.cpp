#include "ViewModel.h"

namespace AM
{
namespace Client
{

ViewModel::ViewModel()
: hoveredEntityTooltipText{""}
, hoveredItemTooltipText{""}
, targetEntity{entt::null}
, tooltipTextUpdatedSig{}
, entityTargetedSig{}
, tooltipTextUpdated{tooltipTextUpdatedSig}
, entityTargeted{entityTargetedSig}
{
}

void ViewModel::setHoveredEntity(std::string_view tooltipText)
{
    hoveredEntityTooltipText = tooltipText;
    tooltipTextUpdatedSig.publish(tooltipText);
}

void ViewModel::clearHoveredEntity()
{
    hoveredEntityTooltipText = "";

    // If an item is hovered, set the tooltip to it.
    // Note: We never can physically hover an item and entity at the same time,
    //       but if you move the cursor directly from an entity to an item, 
    //       we may get the "item hovered" signal before "entity unhovered".
    if (hoveredItemTooltipText != "") {
        tooltipTextUpdatedSig.publish(hoveredItemTooltipText);
    }
    else {
        tooltipTextUpdatedSig.publish("");
    }
}

void ViewModel::setHoveredItem(std::string_view tooltipText)
{
    hoveredItemTooltipText = tooltipText;
    tooltipTextUpdatedSig.publish(tooltipText);
}

void ViewModel::clearHoveredItem()
{
    hoveredItemTooltipText = "";

    // If an entity is hovered, set the tooltip to it.
    if (hoveredEntityTooltipText != "") {
        tooltipTextUpdatedSig.publish(hoveredEntityTooltipText);
    }
    else {
        tooltipTextUpdatedSig.publish("");
    }
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
