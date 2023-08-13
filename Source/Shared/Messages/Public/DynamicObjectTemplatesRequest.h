#pragma once

#include "ProjectMessageType.h"
#include "NetworkDefs.h"

namespace AM
{

/**
 * Used to request the latest dynamic object templates from the server.
 */
struct DynamicObjectTemplatesRequest {
    // The MessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr ProjectMessageType MESSAGE_TYPE{
        ProjectMessageType::DynamicObjectTemplatesRequest};

    // Note: No replicated data.

    //--------------------------------------------------------------------------
    // Non-replicated data
    //--------------------------------------------------------------------------
    /**
     * The network ID of the client that sent this message.
     * Set by the server.
     * No IDs are accepted from the client because we can't trust it,
     * so we fill in the ID based on which socket the message came from.
     */
    NetworkID netID{0};
};

template<typename S>
void serialize(S& serializer, DynamicObjectTemplatesRequest& objectTemplatesRequest)
{
    // Note: No data to serialize.
}

} // End namespace AM
