#pragma once

#include "ConnectionError.h"
#include "QueuedEvents.h"
#include "AUI/Window.h"
#include "AUI/Text.h"
#include "AUI/Image.h"
#include "TitleTextInput.h"
#include "TitleButton.h"
#include <random>

namespace AM
{
class AssetCache;

namespace Client
{
class UserInterfaceExtension;
class TitleScreen;
class WorldSinks;

/**
 * The single window for the title screen.
 */
class TitleWindow : public AUI::Window
{
public:
    TitleWindow(UserInterfaceExtension& inUserInterface,
                WorldSinks& inWorldSinks, EventDispatcher& inUiEventDispatcher,
                AssetCache& inAssetCache);

private:
    // UI event handlers.
    /** Sends a ConnectionRequest to the sim. */
    void onConnectButtonPressed();

    // World signal handlers.
    /** Changes to the main screen.  */
    void onSimulationStarted();
    /** Updates the statusText and re-enables the interactables.  */
    void onServerConnectionError(ConnectionError connectionError);

    /** Returns a random lead-in text for above the title text. */
    std::string getRandomLeadText();

    /** The user interface manager. Used for switching to the main screen. */
    UserInterfaceExtension& userInterface;

    /** Event dispatcher. Used for sending connection requests to the sim. */
    EventDispatcher& uiEventDispatcher;

    /** Used to load textures. */
    AssetCache& assetCache;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    AUI::Text leadText;

    AUI::Text titleText;

    // Disabled until we add usernames
    //AUI::Text userNameLabel;
    //TitleTextInput userNameInput;

    TitleButton connectButton;

    AUI::Text statusText;

    /** Used for randomly picking a lead-in text. */
    std::mt19937 randGenerator;
};

} // End namespace Client
} // End namespace AM
