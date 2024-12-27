#pragma once

#include "BuildTool.h"

namespace AM
{
struct WallGraphicSet;

namespace Client
{

/**
 * The build mode tool used for setting wall tile layers.
 */
class WallTool : public BuildTool
{
public:
    WallTool(World& inWorld, Network& inNetwork);

    void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet) override;

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
    void addPhantomWalls();

    /**
     * Adds a North wall phantom to mouseTilePosition and adds gap fills if
     * necessary.
     */
    void addNorthWallPhantom();

    /**
     * Adds a West wall phantom to mouseTilePosition and adds gap fills if
     * necessary.
     */
    void addWestWallPhantom();

    /**
     * Pushes the given sprite into the phantomTileSprites vector as a phantom
     * of the given wall type
     */
    void pushPhantomWall(const TilePosition& tilePosition, Wall::Type wallType,
                         const WallGraphicSet& wallGraphicSet);

    /** The currently selected sprite set. */
    const WallGraphicSet* selectedGraphicSet;
};

} // End namespace Client
} // End namespace AM
