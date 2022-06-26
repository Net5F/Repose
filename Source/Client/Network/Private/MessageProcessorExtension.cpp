#include "MessageProcessorExtension.h"
#include "DispatchMessage.h"
#include "Log.h"

namespace AM
{
namespace Client
{
MessageProcessorExtension::MessageProcessorExtension([[maybe_unused]] MessageProcessorExDependencies deps) {}

void MessageProcessorExtension::processReceivedMessage([[maybe_unused]] MessageType messageType, [[maybe_unused]] Uint8* messageBuffer,
                            [[maybe_unused]] unsigned int messageSize)
{
}

} // End namespace Client
} // End namespace AM
