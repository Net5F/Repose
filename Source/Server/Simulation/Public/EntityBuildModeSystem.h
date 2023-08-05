#pragma once

#include "EntityTemplatesRequest.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Server
{

class World;
class Network;
class SpriteData;

/**
 * Supports the build mode Entity tool by managing entity templates and 
 * providing script data.
 */
class EntityBuildModeSystem
{
public:
    EntityBuildModeSystem(World& inWorld,
                        EventDispatcher& inNetworkEventDispatcher,
                        Network& inNetwork, SpriteData& inSpriteData);

    /**
     * Adds any waiting templates to the list, and responds to entity template 
     * and script data requests.
     */
    void processEntityBuildModeMessages();

private:
    /** Used to add/remove entities. */
    World& world;

    /** Used to send entity-related messages. */
    Network& network;

    // TODO: Rem?
    /** Used to get sprite data when adding a dynamic object. */
    SpriteData& spriteData;

    EventQueue<EntityTemplatesRequest> templatesRequestQueue;
};

} // End namespace Server
} // End namespace AM
