#pragma once

#include "AUI/Screen.h"
#include "Camera.h"
#include "BuildPanel.h"
#include "BuildOverlay.h"
#include "MainOverlay.h"
#include "TileExtent.h"

namespace AM
{
class EventDispatcher;
class AssetCache;
struct Position;

namespace Client
{
class WorldSinks;
class SpriteData;

/**
 * The main UI that overlays the world.
 */
class MainScreen : public AUI::Screen
{
public:
    MainScreen(WorldSinks& inWorldSinks, EventDispatcher& inUiEventDispatcher,
               AssetCache& inAssetCache, SpriteData& inSpriteData);

    /**
     * Sets the camera to use when rendering.
     *
     * Called by the renderer to give us the lerped camera for the current
     * frame.
     */
    void setCamera(const Camera& inCamera);

    //-------------------------------------------------------------------------
    // Screen class overrides
    //-------------------------------------------------------------------------
    bool onKeyDown(SDL_Keycode keyCode) override;

private:
    /**
     * If the player enters or exits the build area, performs the necessary UI
     * changes.
     */
    void onPlayerPositionChanged(Position position);

    /** If true, the player is currently in the build area. */
    bool playerIsInBuildArea;

    //-------------------------------------------------------------------------
    // Windows
    //-------------------------------------------------------------------------
    /** The main overlay. Currently only shows the "build mode" text, but
        will eventually show the chat box, player names, etc. */
    MainOverlay mainOverlay;

    /** The build mode overlay. Allows the user to place tiles. */
    BuildOverlay buildOverlay;

    /** The build mode panel. Allows the user to select tiles. */
    BuildPanel buildPanel;
};

} // End namespace Client
} // End namespace AM
