#include "MessageProcessorExtension.h"
#include "DispatchMessage.h"
#include "ProjectMessageType.h"
#include "DynamicObjectTemplatesRequest.h"
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
        case ProjectMessageType::DynamicObjectTemplatesRequest: {
            handleEntityTemplatesRequest(netID, messageBuffer,
                                                messageSize);
            dispatchMessage<DynamicObjectTemplatesRequest>(
                messageBuffer, messageSize, networkEventDispatcher);
            break;
        }
        default: {
            LOG_FATAL("Received unexpected message type: %u", messageType);
        }
    }
}

void MessageProcessorExtension::handleEntityTemplatesRequest(
    NetworkID netID, Uint8* messageBuffer, std::size_t messageSize)
{
    // Deserialize the message.
    DynamicObjectTemplatesRequest objectTemplatesRequest{};
    Deserialize::fromBuffer(messageBuffer, messageSize,
                            objectTemplatesRequest);

    // Fill in the network ID that we assigned to this client.
    objectTemplatesRequest.netID = netID;

    // Push the message into any subscribed queues.
    networkEventDispatcher.push<DynamicObjectTemplatesRequest>(
        objectTemplatesRequest);
}

} // End namespace Server
} // End namespace AM
