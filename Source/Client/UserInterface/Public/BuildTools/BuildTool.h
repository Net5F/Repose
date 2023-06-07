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

    BuildTool(EventDispatcher& inUiEventDispatcher);

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
                             const SDL_Point& cursorPosition)
        = 0;
    virtual void onMouseUp(AUI::MouseButtonType buttonType,
                           const SDL_Point& cursorPosition)
        = 0;
    virtual void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition)
        = 0;
    virtual void onMouseWheel(int amountScrolled) = 0;
    virtual void onMouseMove(const SDL_Point& cursorPosition)
        = 0;
    // Note: This is called when the cursor leaves the BuildOverlay.
    virtual void onMouseLeave() = 0;

protected:
    /** Used for sending tile update requests to the sim, which forwards them 
        to the server. */
    EventDispatcher& uiEventDispatcher;

    /** The camera to use when doing screen -> world calcs. */
    Camera camera;

    /** The world tile map's extent. Used to make sure we aren't rendering or
        requesting changes to tiles that are out of bounds. */
    TileExtent mapTileExtent;

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
