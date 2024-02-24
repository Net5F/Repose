#pragma once

#include "GraphicID.h"

namespace AM
{
namespace Client
{

class World;
class GraphicData;

/**
 * Handles updating entity sprites to reflect their current state.
 *
 * TODO: When animation support is added, update this class to handle changing
 *       entity animations to reflect their current state, and to progress any
 *       animations that are ready for their next sprite.
 */
class AnimationSystem
{
public:
    AnimationSystem(World& inWorld, GraphicData& inGraphicData);

    /**
     * Updates each entity's sprite to reflect their current rotation.
     */
    void updateAnimations();

private:
    /** Used to get entity GraphicState components. */
    World& world;

    /** Used to get entity graphics. */
    GraphicData& graphicData;

    /** Used for setting ghost graphics. */
    const GraphicID GHOST_NORTH_ID;
    const GraphicID GHOST_EAST_ID;
    const GraphicID GHOST_SOUTH_ID;
    const GraphicID GHOST_WEST_ID;
};

} // End namespace Client
} // End namespace AM
