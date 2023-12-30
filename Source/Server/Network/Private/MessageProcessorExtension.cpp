#include "MessageProcessorExtension.h"
#include "MessageProcessorExDependencies.h"
#include "DispatchMessage.h"
#include "ProjectMessageType.h"
#include "EntityTemplatesRequest.h"
#include "AddEntityTemplate.h"
#include "Log.h"
#include "QueuedEvents.h"
#include <span>

namespace AM
{
namespace Server
{
template<typename T>
void dispatchWithNetID(NetworkID netID, std::span<Uint8> messageBuffer,
                       EventDispatcher& dispatcher)
{
    // Deserialize the message.
    T message{};
    Deserialize::fromBuffer(messageBuffer.data(), messageBuffer.size(),
                            message);

    // Fill in the network ID that we assigned to this client.
    message.netID = netID;

    // Push the message into any subscribed queues.
    dispatcher.push<T>(message);
}

MessageProcessorExtension::MessageProcessorExtension(
    const MessageProcessorExDependencies& deps)
: networkEventDispatcher{deps.networkEventDispatcher}
{
}

void MessageProcessorExtension::processReceivedMessage(NetworkID netID,
                                                       Uint8 messageType,
                                                       Uint8* messageBuffer,
                                                       std::size_t messageSize)
{
    // Match the enum values to their event types.
    ProjectMessageType projectMessageType{
        static_cast<ProjectMessageType>(messageType)};
    switch (projectMessageType) {
        case ProjectMessageType::EntityTemplatesRequest: {
            dispatchWithNetID<EntityTemplatesRequest>(
                netID, {messageBuffer, messageSize}, networkEventDispatcher);
            break;
        }
        case ProjectMessageType::AddEntityTemplate: {
            dispatchMessage<AddEntityTemplate>(messageBuffer, messageSize,
                                               networkEventDispatcher);
            break;
        }
        default: {
            LOG_FATAL("Received unexpected message type: %u", messageType);
            break;
        }
    }
}

} // End namespace Server
} // End namespace AM
