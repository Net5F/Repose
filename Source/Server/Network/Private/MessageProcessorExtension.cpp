#include "MessageProcessorExtension.h"
#include "DispatchMessage.h"
#include "Log.h"

namespace AM
{
namespace Server
{

MessageProcessorExtension::MessageProcessorExtension([[maybe_unused]] MessageProcessorExDependencies deps) {}

Sint64 MessageProcessorExtension::processReceivedMessage([[maybe_unused]] NetworkID netID, [[maybe_unused]] MessageType messageType,
                             [[maybe_unused]] Uint8* messageBuffer,
                              [[maybe_unused]] unsigned int messageSize)
{
    // The tick that the received message corresponds to.
    // Will be -1 if the message doesn't correspond to any tick.
    Sint64 messageTick{-1};

    return messageTick;
}

} // End namespace Server
} // End namespace AM
