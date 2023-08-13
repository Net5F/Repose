#include "AnimationSystem.h"
#include "World.h"
#include "SpriteData.h"
#include "EntityType.h"
#include "Rotation.h"
#include "Sprite.h"
#include "Log.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{

AnimationSystem::AnimationSystem(World& inWorld, SpriteData& inSpriteData)
: world{inWorld}
, spriteData{inSpriteData}
, GHOST_NORTH_ID{spriteData.getSprite("ghost_north").numericID}
, GHOST_EAST_ID{spriteData.getSprite("ghost_east").numericID}
, GHOST_SOUTH_ID{spriteData.getSprite("ghost_south").numericID}
, GHOST_WEST_ID{spriteData.getSprite("ghost_west").numericID}
{
}

void AnimationSystem::updateAnimations()
{
    // Update all client entity sprites to match current rotation.
    auto view{world.registry.view<EntityType, Rotation, Sprite>()};
    for (entt::entity entity : view) {
        auto [entityType, rotation, sprite]
            = view.get<EntityType, Rotation, Sprite>(entity);

        // TODO: Remove ghost-specific logic and add support for NPCs.
        // If this isn't a client entity, skip it.
        if (entityType != EntityType::ClientEntity) {
            continue;
        }

        // If the new rotation includes the current-faced direction, don't
        // change the sprite.
        int currentSpriteId{sprite.numericID};
        if (currentSpriteId == GHOST_NORTH_ID) {
            if (rotation.direction == Rotation::Direction::North
                || rotation.direction == Rotation::Direction::NorthWest
                || rotation.direction == Rotation::Direction::NorthEast) {
                continue;
            }
        }
        else if (currentSpriteId == GHOST_EAST_ID) {
            if (rotation.direction == Rotation::Direction::East
                || rotation.direction == Rotation::Direction::NorthEast
                || rotation.direction == Rotation::Direction::SouthEast) {
                continue;
            }
        }
        else if (currentSpriteId == GHOST_SOUTH_ID) {
            if (rotation.direction == Rotation::Direction::South
                || rotation.direction == Rotation::Direction::SouthWest
                || rotation.direction == Rotation::Direction::SouthEast) {
                continue;
            }
        }
        else if (currentSpriteId == GHOST_WEST_ID) {
            if (rotation.direction == Rotation::Direction::West
                || rotation.direction == Rotation::Direction::NorthWest
                || rotation.direction == Rotation::Direction::SouthWest) {
                continue;
            }
        }

        // Update the sprite to match the rotation.
        switch (rotation.direction) {
            case Rotation::Direction::North: {
                sprite.numericID = GHOST_NORTH_ID;
                break;
            }
            case Rotation::Direction::NorthEast:
            case Rotation::Direction::SouthEast:
            case Rotation::Direction::East: {
                sprite.numericID = GHOST_EAST_ID;
                break;
            }
            case Rotation::Direction::South: {
                sprite.numericID = GHOST_SOUTH_ID;
                break;
            }
            case Rotation::Direction::NorthWest:
            case Rotation::Direction::SouthWest:
            case Rotation::Direction::West: {
                sprite.numericID = GHOST_WEST_ID;
                break;
            }
            default: {
                break;
            }
        }
    }
}

} // End namespace Client
} // End namespace AM
