#include "MainScreen.h"
#include "World.h"
#include "WorldSinks.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "SpriteData.h"
#include "Paths.h"
#include "BuildModeDefs.h"
#include "SharedConfig.h"
#include "AUI/Core.h"
#include "Log.h"

namespace AM
{
namespace Client
{
MainScreen::MainScreen(const World& inWorld, WorldSinks& inWorldSinks,
                       const WorldObjectLocator& inWorldObjectLocator,
                       EventDispatcher& inUiEventDispatcher,
                       Network& inNetwork,
                       SpriteData& inSpriteData)
: AUI::Screen("MainScreen")
, playerIsInBuildArea{false}
, mainOverlay{}
, buildOverlay{inWorld, inWorldSinks, inWorldObjectLocator, inNetwork}
, buildPanel{inNetwork, inSpriteData, buildOverlay}
{
    // Add our windows so they're included in rendering, etc.
    windows.push_back(mainOverlay);
    windows.push_back(buildOverlay);
    windows.push_back(buildPanel);

    // Deactivate build mode.
    buildOverlay.setIsVisible(false);
    buildPanel.setIsVisible(false);

    // If tile updates are restricted, we need to know when the player enters
    // or exits the build area.
    if (SharedConfig::RESTRICT_TILE_UPDATES) {
        inWorldSinks.playerPositionChanged
            .connect<&MainScreen::onPlayerPositionChanged>(*this);
    }
    else {
        // Tile updates are unrestricted, so the player is always in the build
        // area.
        playerIsInBuildArea = true;
    }
}

void MainScreen::setCamera(const Camera& inCamera)
{
    buildOverlay.setCamera(inCamera);
}

std::vector<PhantomSpriteInfo> MainScreen::getPhantomSprites() const
{
    // Fill a vector with the phantoms from each relevent UI object.
    std::vector<PhantomSpriteInfo> phantoms{};

    std::span<const PhantomSpriteInfo> overlayPhantoms{
        buildOverlay.getPhantomSprites()};
    phantoms.assign(overlayPhantoms.begin(), overlayPhantoms.end());

    return phantoms;
}

std::vector<SpriteColorModInfo> MainScreen::getSpriteColorMods() const
{
    // Fill a vector with the color mods from each relevent UI object.
    std::vector<SpriteColorModInfo> colorMods{};

    std::span<const SpriteColorModInfo> overlayColorMods{
        buildOverlay.getSpriteColorMods()};
    colorMods.assign(overlayColorMods.begin(), overlayColorMods.end());

    return colorMods;
}

bool MainScreen::onKeyDown(SDL_Keycode keyCode)
{
    // If the 'b' key is pressed and the player is in the build area, toggle
    // build mode.
    if ((keyCode == SDLK_b) && playerIsInBuildArea) {
        bool buildModeIsActive{buildOverlay.getIsVisible()};

        buildOverlay.setIsVisible(!buildModeIsActive);
        buildPanel.setIsVisible(!buildModeIsActive);

        return true;
    }

    return false;
}

void MainScreen::onPlayerPositionChanged(Position position)
{
    // If the new position is within the build area, make the hint text visible.
    if (BUILD_MODE_AREA_EXTENT.containsPosition(position.asTilePosition())) {
        if (!playerIsInBuildArea) {
            // The player just entered the build area.
            mainOverlay.setBuildModeHintVisibility(true);

            playerIsInBuildArea = true;
        }
    }
    else {
        if (playerIsInBuildArea) {
            // The player just left the build area.
            mainOverlay.setBuildModeHintVisibility(false);

            // Close build mode, since it isn't available anymore.
            buildOverlay.setIsVisible(false);
            buildPanel.setIsVisible(false);

            playerIsInBuildArea = false;
        }
    }
}

} // End namespace Client
} // End namespace AM
