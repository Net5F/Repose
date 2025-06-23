#pragma once

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
     * Sets the currently hovered entity. To clear, set this to entt::null.
     */
    void setHoveredEntity(entt::entity entity);
    entt::entity getHoveredEntity() const;

    /**
     * Sets the currently targeted entity. To clear, set this to entt::null.
     */
    void setTargetEntity(entt::entity entity);
    entt::entity getTargetEntity() const;

private:
    /** If the mouse is currently hovering over an entity, this is its ID.
        Else, this will be entt::null. */
    entt::entity hoveredEntity;

    /** If an entity is currently targeted, this is it's ID. Else, this will 
        be entt::null. */
    entt::entity targetEntity;

    //-------------------------------------------------------------------------
    // Signals
    //-------------------------------------------------------------------------
    entt::sigh<void(entt::entity newHoveredEntity)> entityHoveredSig;

    entt::sigh<void(entt::entity newTargetedEntity)> entityTargetedSig;

public:
    //-------------------------------------------------------------------------
    // Signal Sinks
    //-------------------------------------------------------------------------
    /** The player has hovered an entity, or is no longer hovering an entity. 
        Will be entt::null if no entity is hovered. */
    entt::sink<entt::sigh<void(entt::entity newHoveredEntity)>> entityHovered;

    /** The player has targeted an entity, or cleared their target. Will be 
        entt::null if the target has been cleared. */
    entt::sink<entt::sigh<void(entt::entity newTargetedEntity)>> entityTargeted;
};

} // End namespace Client
} // End namespace AM
