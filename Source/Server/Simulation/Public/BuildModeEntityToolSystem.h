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
 * Supports the build mode Entity tool by managing entity templates, 
 * providing script data, and creating entities.
 */
class BuildModeEntityToolSystem
{
public:
    BuildModeEntityToolSystem(World& inWorld,
                              EventDispatcher& inNetworkEventDispatcher,
                              Network& inNetwork, SpriteData& inSpriteData);

    /**
     * Adds any waiting templates to the list, responds to entity template 
     * and script data requests, and creates entities on request.
     */
    void processMessages();

private:
    /** Used to add/remove entities. */
    World& world;

    /** Used to send entity-related messages. */
    Network& network;

    // TODO: Remove when we remove the placeholder data?
    SpriteData& spriteData;

    EventQueue<EntityTemplatesRequest> templatesRequestQueue;
};

} // End namespace Server
} // End namespace AM
