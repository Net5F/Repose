#include "AnimationSystem.h"
#include "World.h"
#include "GraphicData.h"
#include "IsClientEntity.h"
#include "Rotation.h"
#include "GraphicState.h"
#include "Log.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{

AnimationSystem::AnimationSystem(World& inWorld, GraphicData& inGraphicData)
: world{inWorld}
, graphicData{inGraphicData}
, GHOST_NORTH_ID{toGraphicID(graphicData.getSprite("ghost_north").numericID)}
, GHOST_EAST_ID{toGraphicID(graphicData.getSprite("ghost_east").numericID)}
, GHOST_SOUTH_ID{toGraphicID(graphicData.getSprite("ghost_south").numericID)}
, GHOST_WEST_ID{toGraphicID(graphicData.getSprite("ghost_west").numericID)}
{
}

void AnimationSystem::updateAnimations()
{
    // Update all client entity sprites to match current rotation.
    auto view{world.registry.view<IsClientEntity, Rotation, GraphicState>()};
    for (auto [entity, rotation, graphicState] : view.each()) {
        // TODO: Remove ghost-specific logic and add support for NPCs.

        // If the new rotation includes the current-faced direction, don't
        // change the sprite.
        const GraphicRef& graphic{
            graphicData.getGraphic(graphicState.graphicSetID)};
        GraphicID currentGraphicID{graphic.getGraphicID()};
        if (currentGraphicID == GHOST_NORTH_ID) {
            if (rotation.direction == Rotation::Direction::North
                || rotation.direction == Rotation::Direction::NorthWest
                || rotation.direction == Rotation::Direction::NorthEast) {
                continue;
            }
        }
        else if (currentGraphicID == GHOST_EAST_ID) {
            if (rotation.direction == Rotation::Direction::East
                || rotation.direction == Rotation::Direction::NorthEast
                || rotation.direction == Rotation::Direction::SouthEast) {
                continue;
            }
        }
        else if (currentGraphicID == GHOST_SOUTH_ID) {
            if (rotation.direction == Rotation::Direction::South
                || rotation.direction == Rotation::Direction::SouthWest
                || rotation.direction == Rotation::Direction::SouthEast) {
                continue;
            }
        }
        else if (currentGraphicID == GHOST_WEST_ID) {
            if (rotation.direction == Rotation::Direction::West
                || rotation.direction == Rotation::Direction::NorthWest
                || rotation.direction == Rotation::Direction::SouthWest) {
                continue;
            }
        }

        // Update the sprite to match the rotation.
        switch (rotation.direction) {
            case Rotation::Direction::North: {
                graphicState.graphicIndex = Rotation::Direction::North;
                break;
            }
            case Rotation::Direction::NorthEast:
            case Rotation::Direction::SouthEast:
            case Rotation::Direction::East: {
                graphicState.graphicIndex = Rotation::Direction::East;
                break;
            }
            case Rotation::Direction::South: {
                graphicState.graphicIndex = Rotation::Direction::South;
                break;
            }
            case Rotation::Direction::NorthWest:
            case Rotation::Direction::SouthWest:
            case Rotation::Direction::West: {
                graphicState.graphicIndex = Rotation::Direction::West;
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
