#include "MainScreen.h"
#include "UserInterfaceExDependencies.h"
#include "Simulation.h"
#include "World.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "GraphicData.h"
#include "Paths.h"
#include "BuildModeDefs.h"
#include "SharedConfig.h"
#include "SdlHelpers.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "Log.h"

namespace AM
{
namespace Client
{
MainScreen::MainScreen(const UserInterfaceExDependencies& deps)
: AUI::Screen("MainScreen")
, world{deps.simulation.getWorld()}
, viewModel{}
, interactionManager{deps.simulation.getWorld(), deps.network, deps.itemData,
                     *this, viewModel}
, playerIsInBuildArea{false}
, dialogueResponseQueue{deps.network.getEventDispatcher()}
, mainOverlay{world, deps.worldObjectLocator, deps.network, viewModel,
              interactionManager}
, chatWindow{deps.simulation, deps.network, deps.sdlRenderer}
, dialogueWindow{world, deps.network}
, inventoryWindow{deps.simulation, deps.network, deps.itemData,
                  deps.iconData,   viewModel,    interactionManager}
, hotbarWindow{world, *this, viewModel}
, buildOverlay{deps.simulation, deps.worldObjectLocator, deps.network,
               deps.graphicData}
, buildPanel{deps.simulation, deps.network,  deps.graphicData,
             deps.itemData,   deps.iconData, buildOverlay}
, rightClickMenu{}
, tooltipWindow{}
{
    // Add our windows so they're included in rendering, etc.
    windows.push_back(mainOverlay);
    windows.push_back(chatWindow);
    windows.push_back(dialogueWindow);
    windows.push_back(inventoryWindow);
    windows.push_back(hotbarWindow);
    windows.push_back(buildOverlay);
    windows.push_back(buildPanel);
    windows.push_back(rightClickMenu);
    windows.push_back(tooltipWindow);

    // Hide the windows that aren't always open.
    buildOverlay.setIsVisible(false);
    buildPanel.setIsVisible(false);
    dialogueWindow.setIsVisible(false);
    inventoryWindow.setIsVisible(false);
    rightClickMenu.setIsVisible(false);
    tooltipWindow.setIsVisible(false);

    // If world changes are restricted, we need to know when the player enters
    // or exits the build area.
    if (SharedConfig::RESTRICT_WORLD_CHANGES) {
        world.registry.on_update<Position>()
            .connect<&MainScreen::onPositionChanged>(*this);
    }
    else {
        // World changes are unrestricted, so the player is always in the build
        // area.
        playerIsInBuildArea = true;
    }

    // If the tooltip text changes, show/hide the tooltip window.
    viewModel.tooltipTextUpdated.connect<&MainScreen::onTooltipTextUpdated>(
        this);
}

void MainScreen::addChatMessage(std::string_view message)
{
    chatWindow.addChatMessage(message);
}

void MainScreen::clearRightClickMenu()
{
    rightClickMenu.clear();
}

void MainScreen::addRightClickMenuAction(std::string_view displayText,
                                         std::function<void(void)> onSelected)
{
    rightClickMenu.addMenuAction(displayText, std::move(onSelected));
}

void MainScreen::openRightClickMenu()
{
    // If the menu isn't already open.
    if (!(rightClickMenu.getIsVisible())) {
        // Get the current mouse position and convert to logical.
        SDL_Point cursorPosition{};
        SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
        cursorPosition = AUI::ScalingHelpers::actualToLogical(cursorPosition);

        // Center the menu where the cursor is.
        SDL_Rect menuExtent{rightClickMenu.getLogicalExtent()};
        menuExtent.x = (cursorPosition.x - (menuExtent.w / 2));
        menuExtent.y = (cursorPosition.y - RightClickMenu::PADDING);
        rightClickMenu.setLogicalExtent(menuExtent);

        // Open the menu.
        rightClickMenu.setIsVisible(true);

        // If the tooltip window is open, close it (we don't want the 
        // right click menu and tooltip window open at the same time).
        // Note: This is a bandaid to fix the tooltip staying up and making 
        //       the user move their cursor to get rid of it. A real solution 
        //       for this would probably involve AUI changes, such as re-
        //       calculating the hover path whenever the layout is invalidated.
        tooltipWindow.setIsVisible(false);
    }
}

bool MainScreen::rightClickMenuIsOpen() const
{
    return rightClickMenu.getIsVisible();
}

void MainScreen::setCamera(const Camera& inCamera)
{
    buildOverlay.setCamera(inCamera);
}

std::vector<PhantomSpriteInfo> MainScreen::getPhantomSprites() const
{
    // Fill a vector with the phantoms from each relevant UI object.
    std::vector<PhantomSpriteInfo> phantoms{};

    std::span<const PhantomSpriteInfo> overlayPhantoms{
        buildOverlay.getPhantomSprites()};
    phantoms.assign(overlayPhantoms.begin(), overlayPhantoms.end());

    return phantoms;
}

std::vector<SpriteColorModInfo> MainScreen::getSpriteColorMods() const
{
    // Fill a vector with the color mods from each relevant UI object.
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
    // If the 'i' key is pressed, toggle the inventory.
    else if (keyCode == SDLK_i) {
        inventoryWindow.setIsVisible(!(inventoryWindow.getIsVisible()));
    }

    // Note: Since HotbarWindow doesn't take focus, we need to manually pass 
    //       unhandled key presses to it.
    AUI::EventResult eventResult{hotbarWindow.onKeyDown(keyCode)};
    return (eventResult.wasHandled);
}

void MainScreen::tick(double timestepS)
{
    // Do the normal tick processing.
    Screen::tick(timestepS);

    // Pass any waiting dialogue messages to DialogueWindow for processing.
    DialogueResponse dialogueResponse{};
    while (dialogueResponseQueue.pop(dialogueResponse)) {
        // If the window is closed, we've started a new dialogue. Clear the 
        // old text and make the window visible.
        if (!(dialogueWindow.getIsVisible())) {
            dialogueWindow.clear();
            dialogueWindow.setIsVisible(true);
        }

        dialogueWindow.processDialogueResponse(dialogueResponse);
    }
}

void MainScreen::onPositionChanged(entt::registry& registry,
                                   entt::entity entity)
{
    // We only care about updates to the player entity.
    if (entity != world.playerEntity) {
        return;
    }

    // If the new position is within the build area, make the hint text visible.
    const Position& position{registry.get<Position>(entity)};
    if (BUILD_MODE_AREA_EXTENT.contains(TilePosition(position))) {
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

void MainScreen::onTooltipTextUpdated(std::string_view newTooltipText)
{
    if (newTooltipText != "") {
        tooltipWindow.text.setText(newTooltipText);
        tooltipWindow.setIsVisible(true);
    }
    else {
        tooltipWindow.setIsVisible(false);
    }
}

} // End namespace Client
} // End namespace AM
