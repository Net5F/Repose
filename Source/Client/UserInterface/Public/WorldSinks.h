#pragma once

#include "entt/signal/sigh.hpp"

#include "ConnectionError.h"
#include "TileExtent.h"
#include "Position.h"

namespace AM
{
namespace Client
{
class WorldSignals;

/**
 * Owns sinks for the World's signals.
 *
 * Allows the UI to connect to the World's signals to listen for updates.
 */
class WorldSinks
{
public:
    WorldSinks(WorldSignals& worldSignals);

    /** We've established a connection with the server and the simulation has
        started running. */
    entt::sink<entt::sigh<void()>> simulationStarted;

    /** Our connection to the server has encountered an error. */
    entt::sink<entt::sigh<void(ConnectionError)>> serverConnectionError;

    /** The tile map's extent (size) has changed. */
    entt::sink<entt::sigh<void(TileExtent)>> tileMapExtentChanged;

    /** The player entity's position has changed. */
    entt::sink<entt::sigh<void(Position)>> playerPositionChanged;
};

} // namespace Client
} // namespace AM
