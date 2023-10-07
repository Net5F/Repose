#pragma once

#include "ProjectMessageType.h"
#include "entt/fwd.hpp"
#include "entt/entity/entity.hpp"

namespace AM
{

/**
 * Sent by a client to request that an entity's info be added as a template.
 */
struct AddEntityTemplate {
    // The ProjectMessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr ProjectMessageType MESSAGE_TYPE{
        ProjectMessageType::AddEntityTemplate};

    /** The entity to turn into a template.
        Must not be a client entity. */
    entt::entity entity{entt::null};
};

template<typename S>
void serialize(S& serializer, AddEntityTemplate& addEntityTemplate)
{
    serializer.value4b(addEntityTemplate.entity);
}

} // End namespace AM
