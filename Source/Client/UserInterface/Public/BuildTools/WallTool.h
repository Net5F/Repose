#pragma once

#include "BuildTool.h"

namespace AM
{
struct WallSpriteSet;

namespace Client 
{

/**
 * The build mode tool used for setting and clearing wall tile layers.
 */
class WallTool : public BuildTool
{
public:
    WallTool(const World& inWorld, EventDispatcher& inUiEventDispatcher);

    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /**
     * Adds phantom walls based on the given mouse position. The phantom's wall 
     * types depend on whether the mouse is closer to the top or left of a tile,
     * and what wall types are already on that tile.
     */
    void addPhantomWalls(const SDL_Point& cursorPosition);

    /**
     * Adds a North wall phantom to the given tile and adds gap fills if 
     * necessary.
     */
    void addNorthWallPhantom(int tileX, int tileY);

    /**
     * Adds a West wall phantom to the given tile and adds gap fills if 
     * necessary.
     */
    void addWestWallPhantom(int tileX, int tileY);

    /**
     * Pushes the given wall sprite type from selectedSpriteSet into the 
     * phantomTileSprites vector.
     */
    void pushPhantomWall(int tileX, int tileY, Wall::Type wallType);

    /** The currently selected sprite set. */
    const WallSpriteSet* selectedSpriteSet;
};

} // End namespace Client
} // End namespace AM
