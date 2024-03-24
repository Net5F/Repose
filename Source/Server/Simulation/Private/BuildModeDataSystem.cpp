#include "BuildModeDataSystem.h"
#include "World.h"
#include "Network.h"
#include "GraphicData.h"
#include "Log.h"

namespace AM
{
namespace Server
{

BuildModeDataSystem::BuildModeDataSystem(
    World& inWorld, EventDispatcher& inNetworkEventDispatcher,
    Network& inNetwork, GraphicData& inGraphicData)
: world{inWorld}
, network{inNetwork}
, graphicData{inGraphicData}
, entityTemplates{}
, entityTemplatesRequestQueue{inNetworkEventDispatcher}
, addEntityTemplateQueue{inNetworkEventDispatcher}
{
    // Load our saved templates.
    // TODO: Replace this placeholder data with real data from a database.
    EntityGraphicSetID graphicSetID{
        graphicData.getEntityGraphicSet("ghost").numericID};
    entityTemplates.templates.emplace_back(Name{"First"},
                                           GraphicState{graphicSetID});
}

void BuildModeDataSystem::processMessages()
{
    // Process any waiting requests to add a template.
    AddEntityTemplate addEntityTemplate{};
    while (addEntityTemplateQueue.pop(addEntityTemplate)) {
        entt::entity entity{addEntityTemplate.entity};
        if (world.registry.valid(entity)) {
            // Collect the entity's relevant data and push it into the list.
            EntityTemplates::Data templateData{};
            if (Name * name{world.registry.try_get<Name>(entity)}) {
                templateData.name = *name;
            }
            if (GraphicState* graphicState{
                    world.registry.try_get<GraphicState>(entity)}) {
                templateData.graphicState = *graphicState;
            }
            if (EntityInitScript
                * initScript{
                    world.registry.try_get<EntityInitScript>(entity)}) {
                templateData.initScript = *initScript;
            }

            entityTemplates.templates.push_back(templateData);
        }
    }

    // Respond to any waiting template data requests.
    EntityTemplatesRequest entityTemplatesRequest{};
    while (entityTemplatesRequestQueue.pop(entityTemplatesRequest)) {
        // Send the latest entity templates to the requesting client.
        network.serializeAndSend(entityTemplatesRequest.netID, entityTemplates);
    }
}

} // End namespace Server
} // End namespace AM
