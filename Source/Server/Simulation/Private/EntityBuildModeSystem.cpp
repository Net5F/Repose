#include "EntityBuildModeSystem.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "EntityTemplates.h"
#include "Log.h"
#include "AMAssert.h"

namespace AM
{
namespace Server
{

EntityBuildModeSystem::EntityBuildModeSystem(
    World& inWorld, EventDispatcher& inNetworkEventDispatcher,
    Network& inNetwork, SpriteData& inSpriteData)
: world{inWorld}
, network{inNetwork}
, spriteData{inSpriteData}
, templatesRequestQueue{inNetworkEventDispatcher}
{
}

void EntityBuildModeSystem::processEntityBuildModeMessages()
{
    // TODO: Add any waiting templates

    // Respond to any waiting template data requests.
    EntityTemplatesRequest templatesRequest{};
    while (templatesRequestQueue.pop(templatesRequest)) {
        // Send the latest entity templates to the requesting client.
        EntityTemplates entityTemplates{};

        // TODO: Replace this placeholder data with real data from the user.
        Uint16 spriteSetID{
            spriteData.getObjectSpriteSet("sunflower").numericID};
        entityTemplates.templates.emplace_back("First", spriteSetID, 0);
        entityTemplates.templates.emplace_back("Second", spriteSetID, 1);
        entityTemplates.templates.emplace_back("Third", spriteSetID, 2);

        network.serializeAndSend(templatesRequest.netID,
                                 entityTemplates);
    }

    // TODO: Respond to any waiting script data requests.
}

} // End namespace Server
} // End namespace AM
