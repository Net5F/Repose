#pragma once

#include "BoundingBox.h"
#include "Vector3.h"
#include "Timer.h"
#include <vector>

namespace AM
{
namespace Server
{

class World;

/**
 * Defines volumes that teleport players to specified coordinates.
 */
class TeleportSystem
{
public:
    TeleportSystem(World& inWorld);

    /**
     * Teleport any players that are in a teleport volume.
     */
    void teleportPlayers();

private:
    /** How often to check if players need to be teleported. */
    static constexpr double UPDATE_TIMESTEP_S{0.2};

    /** Used to modify the tile map. */
    World& world;

    Timer updateTimer;

    /** The teleport volumes that players can walk into. */
    std::vector<BoundingBox> teleportVolumes;

    /** The destination for each volume in teleportVolumes.
        Elements are index-matched with teleportVolumes, e.g. a player
        who enters the volume at teleportVolumes[0] should be teleported to the
        position at teleportDestinations[0]. */
    std::vector<Vector3> teleportDestinations;
};

} // End namespace Server
} // End namespace AM
