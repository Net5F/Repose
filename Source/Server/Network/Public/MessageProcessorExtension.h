#pragma once

#include "IMessageProcessorExtension.h"
#include <SDL_stdinc.h>

namespace AM
{
class EventDispatcher;

namespace Server
{
struct MessageProcessorExDependencies;

/**
 * Processes project messages (i.e. any message that the engine doesn't handle).
 */
class MessageProcessorExtension : public IMessageProcessorExtension
{
public:
    // Note: This is the canonical constructor, expected by the factory that
    //       constructs this class. Do not modify it.
    MessageProcessorExtension(const MessageProcessorExDependencies& deps);

    /**
     * Called when a message is received that the engine doesn't have a handler
     * for.
     */
    void processReceivedMessage(NetworkID netID, Uint8 messageType,
                                Uint8* messageBuffer,
                                std::size_t messageSize) override;

private:
    //-------------------------------------------------------------------------
    // Handlers for messages relevant to the network layer.
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------

    /** The dispatcher for network events. Used to send events to the
        subscribed queues. */
    EventDispatcher& networkEventDispatcher;
};

} // End namespace Server
} // End namespace AM
