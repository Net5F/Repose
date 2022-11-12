#pragma once

namespace AM
{
namespace Client
{

class World;
class SpriteData;

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
    AnimationSystem(World& inWorld, SpriteData& inSpriteData);

    /**
     * Updates each entity's sprite to reflect their current rotation.
     */
    void updateAnimations();

private:
    /** Used to get entity Sprite components. */
    World& world;

    /** Used to get character sprites. */
    SpriteData& spriteData;

    /** Used for setting ghost sprites. */
    const int GHOST_NORTH_ID;
    const int GHOST_EAST_ID;
    const int GHOST_SOUTH_ID;
    const int GHOST_WEST_ID;
};

} // End namespace Client
} // End namespace AM
