#pragma once

#include "EngineReplicatedComponentTypes.h"
#include "boost/mp11/list.hpp"
#include "boost/mp11/algorithm.hpp"

namespace AM
{

/**
 * All of the project's component types that are relevant to the client.
 *
 * When a client comes in range of an entity, an Init message that includes
 * these components will be sent (if the entity possesses any of them).
 *
 * In other words, adding components to this list will cause them to be sent
 * once. If you want a component to additionally be sent whenever it's updated,
 * add it to ProjectObservedComponentTypes.
 */
using ProjectReplicatedComponentTypes = boost::mp11::mp_list<>;

using ReplicatedComponentTypes
    = boost::mp11::mp_append<EngineReplicatedComponentTypes,
                             ProjectReplicatedComponentTypes>;
static_assert(boost::mp11::mp_size<ReplicatedComponentTypes>::value
                  <= (SDL_MAX_UINT8 + 1),
              "Too many types in ReplicatedComponentTypes. Max is 256");

} // End namespace AM
