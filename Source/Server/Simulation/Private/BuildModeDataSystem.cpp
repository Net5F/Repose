#include "BuildModeDataSystem.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "EntityTemplates.h"
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
, entityTemplatesRequestQueue{inNetworkEventDispatcher}
{
}

void BuildModeDataSystem::processMessages()
{
    // TODO: Add any waiting templates

    // Respond to any waiting template data requests.
    EntityTemplatesRequest entityTemplatesRequest{};
    while (entityTemplatesRequestQueue.pop(entityTemplatesRequest)) {
        // Send the latest entity templates to the requesting client.
        EntityTemplates entityTemplates{};

        // TODO: Replace this placeholder data with real data from the user.
        Uint16 spriteSetID{
            spriteData.getObjectSpriteSet("sunflower").numericID};
        entityTemplates.templates.emplace_back(
            Name{"First"},
            AnimationState{SpriteSet::Type::Object, spriteSetID, 0});
        entityTemplates.templates.emplace_back(
            Name{"Second"},
            AnimationState{SpriteSet::Type::Object, spriteSetID, 1});
        entityTemplates.templates.emplace_back(
            Name{"Third"},
            AnimationState{SpriteSet::Type::Object, spriteSetID, 2});

        network.serializeAndSend(entityTemplatesRequest.netID, entityTemplates);
    }

    // TODO: Respond to any waiting script data requests.
}

} // End namespace Server
} // End namespace AM
