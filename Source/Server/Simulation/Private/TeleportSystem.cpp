#include "TeleportSystem.h"
#include "World.h"
#include "BoundingBox.h"
#include "EnttGroups.h"
#include "Transforms.h"
#include "Log.h"

namespace AM
{
namespace Server
{

TeleportSystem::TeleportSystem(World& inWorld)
: world{inWorld}
, updateTimer{}
{
    // Maze -> dev room.
    BoundingBox box{{445.0f, 478.0f, 41.0f}, {44.0f, 0.0f, 1.0f}};
    teleportVolumes.emplace_back(BoundingBox(box));
    teleportDestinations.emplace_back(52.0f, 1973.0f, 0.f);

    // Dev room -> maze.
    box = {{40.0f, 62.0f, 1998.0f}, {2003.0f, 0.0f, 1.0f}};
    teleportVolumes.emplace_back(BoundingBox(box));
    teleportDestinations.emplace_back(462.0f, 77.0f, 0.f);
}

void TeleportSystem::teleportPlayers()
{
    if (updateTimer.getTime() >= UPDATE_TIMESTEP_S) {
        // For each teleport volume.
        for (std::size_t i = 0; i < teleportVolumes.size(); ++i) {
            const BoundingBox& volume{teleportVolumes[i]};

            // Get the list of entities that are touching this volume.
            const auto& collisionMatches{world.collisionLocator.getCollisions(
                volume, (CollisionObjectType::ClientEntity
                         | CollisionObjectType::NonClientEntity))};

            // Teleport each entity to the destination.
            const Vector3& destination{teleportDestinations[i]};
            for (auto* collisionInfo : collisionMatches) {
                world.teleportEntity(collisionInfo->entity, destination);
            }
        }

        updateTimer.reset();
    }
}

} // End namespace Server
} // End namespace AM
