#include "ProjectLuaBindings.h"
#include "World.h"
#include "RandomWalkerAILogic.h"
#include "PreviousPosition.h"
#include "AIBehavior.h"
#include "sol/sol.hpp"

namespace AM
{
namespace Server
{

ProjectLuaBindings::ProjectLuaBindings(sol::state& inLua, World& inWorld)
: lua{inLua}
, world{inWorld}
{
}

void ProjectLuaBindings::addBindings()
{
    lua.set_function("addRandomWalkerAIBehavior",
                     &ProjectLuaBindings::addRandomWalkerAIBehavior, this);
}

void ProjectLuaBindings::addRandomWalkerAIBehavior(
    double timeToWalk, double timeToWait, double timeTillDirectionChange)
{
    // Add any components that this behavior requires.
    entt::entity entity{lua["selfEntityID"]};
    if (!(world.hasMovementComponents(entity))) {
        world.addMovementComponents(entity);
    }

    // Add the behavior.
    auto aiLogic{std::make_unique<RandomWalkerAILogic>(
        world, entity, timeToWalk, timeToWait, timeTillDirectionChange)};
    world.registry.emplace<AIBehavior>(entity, std::move(aiLogic));
}

} // namespace Server
} // namespace AM