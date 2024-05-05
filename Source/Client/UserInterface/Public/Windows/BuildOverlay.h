#pragma once

#include "Camera.h"
#include "TileExtent.h"
#include "GraphicSets.h"
#include "BuildModeType.h"
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
class Simulation;
class World;
class WorldObjectLocator;
class Network;
class GraphicData;
class BuildTool;

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
    BuildOverlay(Simulation& inSimulation,
                 const WorldObjectLocator& inWorldObjectLocator,
                 Network& inNetwork, GraphicData& inGraphicData);

    virtual ~BuildOverlay();

    /**
     * Used by the BuildPanel to tell us when a graphic set is selected.
     */
    void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet);

    /**
     * Used by the BuildPanel to tell us which build mode is currently active.
     *
     * User interactions will be passed to the current tool, which will notify
     * the sim if the tile map needs to change.
     */
    void setBuildMode(BuildMode::Type buildModeType);

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

    /** We hold onto these so we can pass them to the current tool. */
    World& world;
    const WorldObjectLocator& worldObjectLocator;
    Network& network;
    /** Used to get sprites from AnimationStates. */
    GraphicData& graphicData;

    /** The currently selected graphic set. */
    const GraphicSet* selectedGraphicSet;

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
