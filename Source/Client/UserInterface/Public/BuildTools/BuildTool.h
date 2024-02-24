#pragma once

#include "TileExtent.h"
#include "PhantomSpriteInfo.h"
#include "SpriteColorModInfo.h"
#include "Camera.h"
#include "AUI/MouseButtonType.h"
#include <span>
#include <vector>

namespace AM
{
struct GraphicSet;

namespace Client
{
class World;
class Network;

/**
 * Base class for build mode tools that the user can use to modify the world.
 */
class BuildTool
{
public:
    // TODO: We may eventually want to render things like highlight lines
    //       around an extent that we're clearing. We could do so by adding
    //       something like a DisplayInfo struct with a getter that lets the
    //       tool tell the overlay to render things.

    BuildTool(World& inWorld, Network& inNetwork);

    /**
     * Returns any phantom sprites that this build tool wants to render.
     */
    std::span<const PhantomSpriteInfo> getPhantomSprites() const;

    /**
     * Returns any sprite color modifications that this build tool wants to
     * render.
     */
    std::span<const SpriteColorModInfo> getSpriteColorMods() const;

    /**
     * Used by the BuildPanel to tell us when a new graphic set is selected.
     */
    virtual void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet);

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
    /** Used for getting the world state so our tools can make decisions and
        send messages. */
    World& world;

    /** Used for sending tile update requests to the server. */
    Network& network;

    /** The camera to use when doing screen -> world calcs. */
    Camera camera;

    /** The world tile map's extent. Used to make sure we aren't rendering or
        requesting changes to tiles that are out of bounds. */
    TileExtent mapTileExtent;

    /** The world position that the mouse is hovering over. */
    Position mouseWorldPosition;

    /** The tile that the mouse is hovering over. */
    TilePosition mouseTilePosition;

    /** If true, the mouse is inside the world and this tool should be active.
        If false, this tool should not render anything (including phantoms and
        color mods) or respond to inputs. */
    bool isActive;

    /** Holds any phantom sprites that this build tool wants to render.
        These sprites get passed down to the Renderer, which then correctly
        sorts and renders them while rendering the sim's world data. */
    std::vector<PhantomSpriteInfo> phantomSprites;

    /** Holds any of the simulation's sprites that this build tool wants to
        modify the color or transparency of. */
    std::vector<SpriteColorModInfo> spriteColorMods;
};

} // End namespace Client
} // End namespace AM
