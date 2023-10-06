#pragma once

#include "entt/fwd.hpp"

namespace sol
{
class state;
}

namespace AM
{
namespace Server
{
class World;

/**
 * Holds any functionality that the project wants to expose to Lua.
 * 
 * Note: This is a class instead of a set of free functions, because it's more 
 *       convenient for the bound functions to have access to some state.
 */
class ProjectLuaBindings
{
public:
    ProjectLuaBindings(sol::state& inLua, World& inWorld);

    /**
     * Adds our bindings to the lua object.
     */
    void addBindings();

private:
    sol::state& lua;
    World& world;

    /**
     * Makes the entity walk around randomly.
     * @param timeToWalk How long to walk for.
     * @param timeToWalk How long to wait for.
     * @param timeTillDirectionChange How often to change direction.
     */
    void addRandomWalkerAIBehavior(double timeToWalk, double timeToWait,
                                   double timeTillDirectionChange);
};

} // namespace Server
} // namespace AM
