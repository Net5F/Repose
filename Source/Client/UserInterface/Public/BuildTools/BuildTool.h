#pragma once

#include "EmptySpriteID.h"
#include "TileExtent.h"
#include "PhantomTileSpriteInfo.h"
#include "TileSpriteColorModInfo.h"
#include "Camera.h"
#include "AUI/MouseButtonType.h"
#include <SDL_stdinc.h>
#include <SDL_rect.h>
#include <span>
#include <vector>

namespace AM
{
class EventDispatcher;
struct SpriteSet;

namespace Client
{
class World;

/**
 * Base class for build mode tools that the user can use to modify the world.
 */
class BuildTool {
public:
    /** The types of tools that our build mode supports. */
    enum Type : Uint8 { 
        Floor,
        FloorCovering,
        Wall,
        Object,
        Remove,
        Count,
        None
    };

    // TODO: We may eventually want to render things like highlight lines 
    //       around an extent that we're clearing. We could do so by adding 
    //       something like a DisplayInfo struct with a getter that lets the 
    //       tool tell the overlay to render things.

    BuildTool(const World& inWorld, EventDispatcher& inUiEventDispatcher);

    /**
     * Returns any phantom tile sprites that this build tool wants to render.
     */
    std::span<const PhantomTileSpriteInfo> getPhantomTileSprites() const;

    /**
     * Returns any tile sprite color modifications that this build tool wants 
     * to render.
     */
    std::span<const TileSpriteColorModInfo> getTileSpriteColorMods() const;

    /**
     * Used by the BuildPanel to tell us when a new sprite set is selected.
     */
    virtual void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) = 0;

    /**
     * Sets the camera to use when doing screen -> world calcs.
     *
     * Called during the render pass to give us the lerped camera for the 
     * current frame.
     */
    void setCamera(const Camera& inCamera);

    /**
     * Used by the sim to tell us when the map size changes.
     */
    void setTileMapExtent(const TileExtent& inTileExtent);

    // Note: The BuildOverlay forwards user events to the current tool through 
    //       these functions. We do it this way because it wouldn't make sense 
    //       for a build tool to be a widget that gets clicked.
    virtual void onMouseDown(AUI::MouseButtonType buttonType,
                             const SDL_Point& cursorPosition);
    virtual void onMouseUp(AUI::MouseButtonType buttonType,
                           const SDL_Point& cursorPosition);
    virtual void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition);
    virtual void onMouseWheel(int amountScrolled);
    virtual void onMouseMove(const SDL_Point& cursorPosition);
    // Note: This is called when the cursor leaves the BuildOverlay.
    virtual void onMouseLeave();

protected:
    /** Used for getting the world's tile map state so this tool can make 
        decisions about what sprites need to be used. */
    const World& world;

    /** Used for sending tile update requests to the sim, which forwards them 
        to the server. */
    EventDispatcher& uiEventDispatcher;

    /** The camera to use when doing screen -> world calcs. */
    Camera camera;

    /** The world tile map's extent. Used to make sure we aren't rendering or
        requesting changes to tiles that are out of bounds. */
    TileExtent mapTileExtent;

    /** The tile that the mouse is hovering over. */
    TilePosition mouseTilePosition;

    /** If true, the mouse is inside the world and this tool should be active. 
        If false, this tool should not render anything (including phantoms and 
        color mods) or respond to inputs. */
    bool isActive;

    /** Holds any phantom tile sprites that this build tool wants to render.
        These sprites get passed down to the Renderer, which then correctly 
        sorts and renders them while rendering the sim's world data. */
    std::vector<PhantomTileSpriteInfo> phantomTileSprites;

    /** Holds any of the simulation's tile layer sprites that this build tool 
        wants to modify the color or transparency of. */
    std::vector<TileSpriteColorModInfo> tileSpriteColorMods;
};

} // End namespace Client
} // End namespace AM