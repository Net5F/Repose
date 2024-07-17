#include "TeleportSystem.h"
#include "World.h"
#include "BoundingBox.h"
#include "EnttGroups.h"
#include "MinMaxBox.h"
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
    MinMaxBox box{{445.0f, 478.0f, 41.0f}, {44.0f, 0.0f, 1.0f}};
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
        auto movementGroup{EnttGroups::getMovementGroup(world.registry)};

        // For each teleport volume.
        for (std::size_t i = 0; i < teleportVolumes.size(); ++i) {
            const BoundingBox& volume{teleportVolumes[i]};

            // Get the list of entities that are touching this volume.
            std::vector<entt::entity>& entitiesIntersectingVolume{
                world.entityLocator.getCollisions(volume)};

            // Teleport each entity to the destination.
            const Vector3& destination{teleportDestinations[i]};
            for (entt::entity entity : entitiesIntersectingVolume) {
                auto [position, collision]
                    = movementGroup.get<Position, Collision>(entity);
                position = destination;
                collision.worldBounds = Transforms::modelToWorldEntity(
                    collision.modelBounds, position);

                // Flag that the entity's movement state needs to be synced.
                // (movement state is auto-synced when Input is dirtied).
                world.registry.patch<Input>(entity, [](auto&) {});
            }
        }

        updateTimer.reset();
    }
}

} // End namespace Server
} // End namespace AM
