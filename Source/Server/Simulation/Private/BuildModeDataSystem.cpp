#include "BuildModeDataSystem.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "DynamicObjectTemplates.h"
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
, objectTemplatesRequestQueue{inNetworkEventDispatcher}
{
}

void BuildModeDataSystem::processMessages()
{
    // TODO: Add any waiting templates

    // Respond to any waiting template data requests.
    DynamicObjectTemplatesRequest objectTemplatesRequest{};
    while (objectTemplatesRequestQueue.pop(objectTemplatesRequest)) {
        // Send the latest dynamic object templates to the requesting client.
        DynamicObjectTemplates objectTemplates{};

        // TODO: Replace this placeholder data with real data from the user.
        Uint16 spriteSetID{
            spriteData.getObjectSpriteSet("sunflower").numericID};
        objectTemplates.templates.emplace_back(
            "First", Rotation{Rotation::Direction::South}, spriteSetID);
        objectTemplates.templates.emplace_back(
            "Second", Rotation{Rotation::Direction::SouthWest}, spriteSetID);
        objectTemplates.templates.emplace_back(
            "Third", Rotation{Rotation::Direction::West}, spriteSetID);

        network.serializeAndSend(objectTemplatesRequest.netID,
                                 objectTemplates);
    }

    // TODO: Respond to any waiting script data requests.
}

} // End namespace Server
} // End namespace AM
