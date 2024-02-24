#pragma once

#include "EntityTemplates.h"
#include "EntityTemplatesRequest.h"
#include "AddEntityTemplate.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Server
{

class World;
class Network;
class GraphicData;

/**
 * Responds to requests for template and script data, for use in a client's
 * build mode.
 */
class BuildModeDataSystem
{
public:
    BuildModeDataSystem(World& inWorld,
                        EventDispatcher& inNetworkEventDispatcher,
                        Network& inNetwork, GraphicData& inGraphicData);

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
    GraphicData& graphicData;

    /** Our current list of entity templates. */
    EntityTemplates entityTemplates;

    EventQueue<EntityTemplatesRequest> entityTemplatesRequestQueue;
    EventQueue<AddEntityTemplate> addEntityTemplateQueue;
};

} // End namespace Server
} // End namespace AM
