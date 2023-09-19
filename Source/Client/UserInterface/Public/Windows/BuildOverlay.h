#pragma once

#include "Camera.h"
#include "TileExtent.h"
#include "TileLayers.h"
#include "SpriteSets.h"
#include "BuildTool.h"
#include "PhantomSpriteInfo.h"
#include "SpriteColorModInfo.h"
#include "AUI/Window.h"
#include <span>
#include <memory>

namespace AM
{
struct Sprite;

namespace Client
{
class World;
class WorldSinks;
class WorldObjectLocator;
class Network;
class SpriteData;

/**
 * The build mode overlay on the main screen. Allows the user to place tiles
 * in the world.
 *
 * This overlay is opened alongside BuildPanel when we enter build mode.
 *
 * Note: This overlay blocks all inputs that it receives. If this becomes 
 *       undesirable, we can change the build tool to say whether it handled 
 *       the event or not.
 */
class BuildOverlay : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    BuildOverlay(World& inWorld, WorldSinks& inWorldSinks,
                 const WorldObjectLocator& inWorldObjectLocator,
                 Network& inNetwork, SpriteData& inSpriteData);

    virtual ~BuildOverlay() = default;

    /**
     * Used by the BuildPanel to tell us when a sprite set is selected.
     */
    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet);

    /**
     * Used by the BuildPanel to tell us which build tool is currently selected.
     *
     * User interactions will be passed to the current tool, which will notify 
     * the sim if the tile map needs to change.
     */
    void setBuildTool(BuildTool::Type toolType);

    /**
     * Returns the current build tool.
     */
    BuildTool* getCurrentBuildTool();

    /**
     * Sets the camera to use when rendering.
     *
     * Called during the render pass to give us the lerped camera for the 
     * current frame.
     */
    void setCamera(const Camera& inCamera);

    /**
     * Returns any phantom tile sprites that the current build tool wants to 
     * render.
     */
    std::span<const PhantomSpriteInfo> getPhantomSprites() const;

    /**
     * Returns any tile sprite color mods that the current build tool wants to 
     * render.
     */
    std::span<const SpriteColorModInfo> getSpriteColorMods() const;

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void render() override;

    AUI::EventResult onMouseDown(AUI::MouseButtonType buttonType,
                                 const SDL_Point& cursorPosition) override;

    AUI::EventResult onMouseUp(AUI::MouseButtonType buttonType,
                               const SDL_Point& cursorPosition) override;

    AUI::EventResult
        onMouseDoubleClick(AUI::MouseButtonType buttonType,
                           const SDL_Point& cursorPosition) override;

    AUI::EventResult onMouseWheel(int amountScrolled) override;

    AUI::EventResult onMouseMove(const SDL_Point& cursorPosition) override;

    void onMouseLeave() override;

private:
    // World signal handlers.
    /**
     * Sets mapTileExtent to the new extent of the tile map.
     */
    void onTileMapExtentChanged(TileExtent inTileExtent);

    /** We hold onto this so we can pass it to the current tool. */
    World& world;
    /** We hold onto this so we can pass it to the current tool. */
    const WorldObjectLocator& worldObjectLocator;
    /** We hold onto this so we can pass it to the current tool. */
    Network& network;
    /** Used to get sprites from AnimationStates. */
    SpriteData& spriteData;

    /** The currently selected sprite set. */
    const SpriteSet* selectedSpriteSet;

    /** The current build tool. */
    std::unique_ptr<BuildTool> currentBuildTool;

    /** The camera to use when rendering or doing screen -> world calcs. */
    Camera camera;

    /** The world tile map's extent. Used to make sure we aren't rendering or
        requesting changes to tiles that are out of bounds. */
    TileExtent mapTileExtent;

    /** The the tile that the mouse cursor is currently hovering over. */
    TilePosition cursorTilePosition;
};

} // End namespace Client
} // End namespace AM
