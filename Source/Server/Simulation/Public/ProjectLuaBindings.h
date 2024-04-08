#pragma once

#include "entt/fwd.hpp"

namespace AM
{
namespace Server
{
class World;
struct EntityInitLua;
struct EntityItemHandlerLua;
struct ItemInitLua;

/**
 * Holds any functionality that the project wants to expose to Lua.
 *
 * Note: This is a class instead of a set of free functions, because it's more
 *       convenient for the bound functions to have access to some state.
 */
class ProjectLuaBindings
{
public:
    ProjectLuaBindings(EntityInitLua& inEntityInitLua,
                       EntityItemHandlerLua& inEntityItemHandlerLua,
                       ItemInitLua& inItemInitLua, World& inWorld);

    /**
     * Adds our bindings to the lua object.
     */
    void addBindings();

private:
    // Entity init
    /**
     * Makes the entity walk around randomly.
     * @param timeToWalk How long to walk for.
     * @param timeToWalk How long to wait for.
     * @param timeTillDirectionChange How often to change direction.
     */
    void addRandomWalkerAIBehavior(double timeToWalk, double timeToWait,
                                   double timeTillDirectionChange);

    // Entity item handler

    // Item init

    EntityInitLua& entityInitLua;
    EntityItemHandlerLua& entityItemHandlerLua;
    ItemInitLua& itemInitLua;
    World& world;
};

} // namespace Server
} // namespace AM
