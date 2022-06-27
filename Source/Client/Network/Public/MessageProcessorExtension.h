#pragma once

#include "IMessageProcessorExtension.h"
#include "MessageProcessorExDependencies.h"

namespace AM
{
namespace Client
{

/**
 * PLACEHOLDER - Currently not rendering anything at the project level.
 */
class MessageProcessorExtension : public IMessageProcessorExtension
{
public:
    // Note: This is the canonical constructor, expected by the factory that
    //       constructs this class. Do not modify it.
    MessageProcessorExtension(MessageProcessorExDependencies deps);

    /**
     * Called when a message is received that the engine doesn't have a handler
     * for.
     */
    void processReceivedMessage(MessageType messageType, Uint8* messageBuffer,
                                unsigned int messageSize) override;

private:
};

} // End namespace Client
} // End namespace AM
