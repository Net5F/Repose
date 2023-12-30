#include "BuildModeDataSystem.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "Log.h"

namespace AM
{
namespace Server
{

BuildModeDataSystem::BuildModeDataSystem(
    World& inWorld, EventDispatcher& inNetworkEventDispatcher,
    Network& inNetwork, SpriteData& inSpriteData)
: world{inWorld}
, network{inNetwork}
, spriteData{inSpriteData}
, entityTemplates{}
, entityTemplatesRequestQueue{inNetworkEventDispatcher}
, addEntityTemplateQueue{inNetworkEventDispatcher}
{
    // Load our saved templates.
    // TODO: Replace this placeholder data with real data from a database.
    Uint16 spriteSetID{spriteData.getObjectSpriteSet("sunflower").numericID};
    entityTemplates.templates.emplace_back(
        Name{"First"}, AnimationState{SpriteSet::Type::Object, spriteSetID, 0});
    entityTemplates.templates.emplace_back(
        Name{"Second"},
        AnimationState{SpriteSet::Type::Object, spriteSetID, 1});
    entityTemplates.templates.emplace_back(
        Name{"Third"}, AnimationState{SpriteSet::Type::Object, spriteSetID, 2});
}

void BuildModeDataSystem::processMessages()
{
    // Process any waiting requests to add a template.
    AddEntityTemplate addEntityTemplate{};
    while (addEntityTemplateQueue.pop(addEntityTemplate)) {
        entt::entity entity{addEntityTemplate.entity};
        if (world.entityIDIsInUse(entity)) {
            // Collect the entity's relevant data and push it into the list.
            EntityTemplates::Data templateData{};
            if (Name * name{world.registry.try_get<Name>(entity)}) {
                templateData.name = *name;
            }
            if (AnimationState
                * animationState{
                    world.registry.try_get<AnimationState>(entity)}) {
                templateData.animationState = *animationState;
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
