#pragma once

#include "entt/fwd.hpp"
#include <string_view> 

namespace AM
{
namespace Server
{
struct EntityInitLua;
struct EntityItemHandlerLua;
struct ItemInitLua;
struct DialogueLua;
struct DialogueChoiceConditionLua;
class GraphicData;
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
    ProjectLuaBindings(EntityInitLua& inEntityInitLua,
                       EntityItemHandlerLua& inEntityItemHandlerLua,
                       ItemInitLua& inItemInitLua, DialogueLua& inDialogueLua,
                       DialogueChoiceConditionLua& inDialogueChoiceConditionLua,
                       const GraphicData& inGraphicData, World& inWorld);

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
    /**
     * Adds a "Test AV Sequence" interaction.
     */
    void addAVSequenceTestInteraction();

    EntityInitLua& entityInitLua;
    EntityItemHandlerLua& entityItemHandlerLua;
    ItemInitLua& itemInitLua;
    DialogueLua& dialogueLua;
    DialogueChoiceConditionLua& dialogueChoiceConditionLua;
    const GraphicData& graphicData;
    World& world;
};

} // namespace Server
} // namespace AM
