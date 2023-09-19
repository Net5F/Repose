#include "MessageProcessorExtension.h"
#include "DispatchMessage.h"
#include "ProjectMessageType.h"
#include "EntityTemplatesRequest.h"
#include "Log.h"

namespace AM
{
namespace Server
{

MessageProcessorExtension::MessageProcessorExtension(
    MessageProcessorExDependencies deps)
: networkEventDispatcher{deps.networkEventDispatcher}
{
}

void MessageProcessorExtension::processReceivedMessage(
    NetworkID netID, Uint8 messageType, Uint8* messageBuffer,
    std::size_t messageSize)
{
    // Match the enum values to their event types.
    ProjectMessageType projectMessageType{
        static_cast<ProjectMessageType>(messageType)};
    switch (projectMessageType) {
        case ProjectMessageType::EntityTemplatesRequest: {
            handleEntityTemplatesRequest(netID, messageBuffer,
                                                messageSize);
            dispatchMessage<EntityTemplatesRequest>(
                messageBuffer, messageSize, networkEventDispatcher);
            break;
        }
        default: {
            LOG_FATAL("Received unexpected message type: %u", messageType);
            break;
        }
    }
}

void MessageProcessorExtension::handleEntityTemplatesRequest(
    NetworkID netID, Uint8* messageBuffer, std::size_t messageSize)
{
    // Deserialize the message.
    EntityTemplatesRequest entityTemplatesRequest{};
    Deserialize::fromBuffer(messageBuffer, messageSize,
                            entityTemplatesRequest);

    // Fill in the network ID that we assigned to this client.
    entityTemplatesRequest.netID = netID;

    // Push the message into any subscribed queues.
    networkEventDispatcher.push<EntityTemplatesRequest>(
        entityTemplatesRequest);
}

} // End namespace Server
} // End namespace AM
