#pragma once

#include "AUI/Screen.h"
#include "Camera.h"
#include "PhantomSpriteInfo.h"
#include "SpriteColorModInfo.h"
#include "ViewModel.h"
#include "InteractionManager.h"
#include "MainOverlay.h"
#include "ChatWindow.h"
#include "DialogueWindow.h"
#include "InventoryWindow.h"
#include "HotbarWindow.h"
#include "BuildPanel.h"
#include "BuildOverlay.h"
#include "RightClickMenu.h"
#include "TooltipWindow.h"
#include "DialogueResponse.h"
#include "QueuedEvents.h"
#include <string_view>
#include <functional>

namespace AM
{
struct Position;

namespace Client
{
struct UserInterfaceExDependencies;

/**
 * The main UI that overlays the world.
 */
class MainScreen : public AUI::Screen
{
public:
    MainScreen(const UserInterfaceExDependencies& deps);

    virtual ~MainScreen() = default;

    /**
     * Adds the given message to the chat.
     */
    void addChatMessage(std::string_view message);

    /**
     * Clears all actions from the right-click menu.
     */
    void clearRightClickMenu();

    /**
     * Adds an action to the right-click menu.
     *
     * @param displayText The text to display for this menu option.
     * @param onSelected The function to call when this option is selected.
     */
    void addRightClickMenuAction(std::string_view displayText,
                                 std::function<void(void)> onSelected);

    /**
     * Moves the right-click menu to the mouse's current position and opens it.
     */
    void openRightClickMenu();

    /**
     * @return true if the right-click menu is open, else false.
     */
    bool rightClickMenuIsOpen() const;

    /**
     * Sets the camera to use when rendering.
     *
     * Called during the render pass to give us the lerped camera for the
     * current frame.
     */
    void setCamera(const Camera& inCamera);

    /**
     * See UserInterfaceExtension.h
     */
    std::vector<PhantomSpriteInfo> getPhantomSprites() const;

    /**
     * See UserInterfaceExtension.h
     */
    std::vector<SpriteColorModInfo> getSpriteColorMods() const;

    //-------------------------------------------------------------------------
    // Screen class overrides
    //-------------------------------------------------------------------------
    bool onKeyDown(SDL_Keycode keyCode) override;

    void tick(double timestepS) override;

private:
    /**
     * If the player enters or exits the build area, performs the necessary UI
     * changes.
     */
    void onPositionChanged(entt::registry& registry, entt::entity entity);

    /**
     * Shows/hides the TooltipWindow.
     */
    void onTooltipTextUpdated(std::string_view newTooltipText);

    /** Used to get the player entity's ID. */
    World& world;

    /** Holds UI-specific model data. */
    ViewModel viewModel;

    /** Orchestrates item and entity interactions from MainOverlay and
        InventoryWindow. */
    InteractionManager interactionManager;

    /** If true, the player is currently in the build area. */
    bool playerIsInBuildArea;

    EventQueue<DialogueResponse> dialogueResponseQueue;

    //-------------------------------------------------------------------------
    // Windows
    //-------------------------------------------------------------------------
    /** The main overlay. Currently only shows the "build mode" text, but
        will eventually show player names, etc. */
    MainOverlay mainOverlay;

    /** The chat window. Currently only shows system messages, but will
        eventually show player messages and support sending messages. */
    ChatWindow chatWindow;

    /** The dialogue window that pops up when you talk to an entity. */
    DialogueWindow dialogueWindow;

    /** The player's inventory window. */
    InventoryWindow inventoryWindow;

    /** The player's hotbar window. */
    HotbarWindow hotbarWindow;

    /** The build mode overlay. Allows the player to place tiles. */
    BuildOverlay buildOverlay;

    /** The build mode panel. Allows the player to select tiles. */
    BuildPanel buildPanel;

    /** A general-purpose right-click menu. Used for e.g. displaying
        the supported interactions when a user right-clicks an item. */
    RightClickMenu rightClickMenu;

    /** A general-purpose tooltip window. Used for e.g. displaying an entity 
        or item name when hovered. */
    TooltipWindow tooltipWindow;
};

} // End namespace Client
} // End namespace AM
