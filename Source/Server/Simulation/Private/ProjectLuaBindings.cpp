#include "ProjectLuaBindings.h"
#include "EntityInitLua.h"
#include "EntityItemHandlerLua.h"
#include "ItemInitLua.h"
#include "World.h"
#include "RandomWalkerAI.h"
#include "PreviousPosition.h"

namespace AM
{
namespace Server
{

ProjectLuaBindings::ProjectLuaBindings(
    EntityInitLua& inEntityInitLua,
    EntityItemHandlerLua& inEntityItemHandlerLua, ItemInitLua& inItemInitLua,
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
    entityInitLua.luaState.set_function(
        "addRandomWalkerAIBehavior",
        &ProjectLuaBindings::addRandomWalkerAIBehavior, this);

    // Entity item handler

    // Item init
}

void ProjectLuaBindings::addRandomWalkerAIBehavior(
    double timeToWalk, double timeToWait, double timeTillDirectionChange)
{
    // Add any components that this behavior requires.
    entt::entity entity{entityInitLua.selfEntity};
    world.addMovementComponents(entity, Rotation{});

    // Add the behavior.
    world.registry.emplace<RandomWalkerAI>(entity, timeToWalk, timeToWait,
                                           timeTillDirectionChange);
}

} // namespace Server
} // namespace AM
