#pragma once

#include <string_view>
#include <string>
#include "entt/fwd.hpp"
#include "entt/entity/entity.hpp"
#include "entt/signal/sigh.hpp"

namespace AM
{
namespace Client
{
/**
 * Holds data that must be shared between UI widgets.
 */
class ViewModel
{
public:
    ViewModel();

    /**
     * Sets the currently hovered entity.
     * @param tooltipText The tooltip text to display while this entity is 
     *                    hovered.
     */
    void setHoveredEntity(std::string_view tooltipText);

    /**
     * Clears the currently hovered entity.
     */
    void clearHoveredEntity();

    /**
     * Sets the currently hovered item.
     * @param tooltipText The tooltip text to display while this item is 
     *                    hovered.
     */
    void setHoveredItem(std::string_view tooltipText);

    /**
     * Clears the currently hovered item.
     */
    void clearHoveredItem();

    /**
     * Sets the currently targeted entity. To clear, set this to entt::null.
     */
    void setTargetEntity(entt::entity entity);
    entt::entity getTargetEntity() const;

private:
    /** If the mouse is currently hovering over an entity, this is the tooltip 
        text that should be displayed. Else, this will == "". */
    std::string hoveredEntityTooltipText;

    /** If the mouse is currently hovering over an item, this is the tooltip 
        text that should be displayed. Else, this will == "". */
    std::string hoveredItemTooltipText;

    /** If an entity is currently targeted, this is it's ID. Else, this will 
        be entt::null. */
    entt::entity targetEntity;

    //-------------------------------------------------------------------------
    // Signals
    //-------------------------------------------------------------------------
    entt::sigh<void(std::string_view newTooltipText)> tooltipTextUpdatedSig;

    entt::sigh<void(entt::entity newTargetedEntity)> entityTargetedSig;

public:
    //-------------------------------------------------------------------------
    // Signal Sinks
    //-------------------------------------------------------------------------
    /** An object has been hovered or unhovered and the tooltip text should 
        be changed. Will == "" if no objects are currently hovered. */
    entt::sink<entt::sigh<void(std::string_view newTooltipText)>>
        tooltipTextUpdated;

    /** The player has targeted an entity, or cleared their target. Will be 
        entt::null if the target has been cleared. */
    entt::sink<entt::sigh<void(entt::entity newTargetedEntity)>> entityTargeted;
};

} // End namespace Client
} // End namespace AM
