#include "ProjectLuaBindings.h"
#include "World.h"
#include "RandomWalkerAI.h"
#include "PreviousPosition.h"
#include "sol/sol.hpp"

namespace AM
{
namespace Server
{

ProjectLuaBindings::ProjectLuaBindings(sol::state& inEntityInitLua,
                                       sol::state& inEntityItemHandlerLua,
                                       sol::state& inItemInitLua,
                                       World& inWorld)
: entityInitLua{inEntityInitLua}
, entityItemHandlerLua{inEntityItemHandlerLua}
, itemInitLua{inItemInitLua}
, world{inWorld}
{
}

void ProjectLuaBindings::addBindings()
{
    // Entity init
    entityInitLua.set_function("addRandomWalkerAIBehavior",
                               &ProjectLuaBindings::addRandomWalkerAIBehavior,
                               this);

    // Entity item handler

    // Item init
}

void ProjectLuaBindings::addRandomWalkerAIBehavior(
    double timeToWalk, double timeToWait, double timeTillDirectionChange)
{
    // Add any components that this behavior requires.
    entt::entity entity{entityInitLua["selfEntityID"]};
    if (!(world.hasMovementComponents(entity))) {
        world.addMovementComponents(entity);
    }

    // Add the behavior.
    world.registry.emplace<RandomWalkerAI>(entity, timeToWalk, timeToWait,
                                           timeTillDirectionChange);
}

} // namespace Server
} // namespace AM
