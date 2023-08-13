#include "MessageProcessorExtension.h"
#include "DispatchMessage.h"
#include "ProjectMessageType.h"
#include "DynamicObjectTemplates.h"
#include "Log.h"

namespace AM
{
namespace Client
{

MessageProcessorExtension::MessageProcessorExtension(
    MessageProcessorExDependencies deps)
: networkEventDispatcher{deps.networkEventDispatcher}
{
}

void MessageProcessorExtension::processReceivedMessage(Uint8 messageType,
                                                       Uint8* messageBuffer,
                                                       std::size_t messageSize)
{
    // Match the enum values to their event types.
    ProjectMessageType projectMessageType{
        static_cast<ProjectMessageType>(messageType)};
    switch (projectMessageType) {
        case ProjectMessageType::DynamicObjectTemplates: {
            dispatchMessage<DynamicObjectTemplates>(
                messageBuffer, messageSize, networkEventDispatcher);
            break;
        }
        default: {
            LOG_FATAL("Received unexpected message type: %u", messageType);
        }
    }
}

} // End namespace Client
} // End namespace AM
