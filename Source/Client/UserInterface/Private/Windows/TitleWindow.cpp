#include "TitleWindow.h"
#include "UserInterfaceExtension.h"
#include "AssetCache.h"
#include "TitleScreen.h"
#include "ConnectionRequest.h"
#include "Paths.h"
#include "Log.h"

namespace AM
{
namespace Client
{
TitleWindow::TitleWindow(UserInterfaceExtension& inUserInterface,
                         WorldSinks& inWorldSinks,
                         EventDispatcher& inUiEventDispatcher,
                         AssetCache& inAssetCache)
: AUI::Window({0, 0, 1920, 1080}, "TitleWindow")
, userInterface{inUserInterface}
, uiEventDispatcher{inUiEventDispatcher}
, assetCache{inAssetCache}
, titleText({0, 271, 1920, 75}, "TitleText")
, userNameLabel({0, 435, 1920, 36}, "UserNameLabel")
, userNameInput(assetCache, {770, 481, 380, 54}, "UserNameInput")
, connectButton(assetCache, {760, 571, 404, 70}, "Connect", "ConnectButton")
, statusText({0, 695, 1920, 48}, "StatusText")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(titleText);
    children.push_back(userNameLabel);
    children.push_back(userNameInput);
    children.push_back(connectButton);
    children.push_back(statusText);

    /* Title text. */
    titleText.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 54);
    titleText.setColor({255, 255, 255, 255});
    titleText.setText("The World of Repose");
    titleText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    /* User name entry. */
    userNameLabel.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 30);
    userNameLabel.setColor({255, 255, 255, 255});
    userNameLabel.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    userNameLabel.setText("User Name");

    userNameInput.setTextFont((Paths::FONT_DIR + "B612-Regular.ttf"), 28);
    userNameInput.setMargins({12, 0, 12, 0});

    /* Status text. */
    statusText.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 30);
    statusText.setColor({255, 255, 255, 255});
    statusText.setText("");
    statusText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    statusText.setIsVisible(true);

    // Register our UI event handlers.
    connectButton.setOnPressed(std::bind(&TitleWindow::onConnectButtonPressed, this));

    // Register our world signal handlers.
    inWorldSinks.simulationStarted.connect<&TitleWindow::onSimulationStarted>(
        *this);
    inWorldSinks.serverConnectionError
        .connect<&TitleWindow::onServerConnectionError>(*this);
}

void TitleWindow::onConnectButtonPressed()
{
    // Communicate that we're attempting to connect and disable interactables.
    statusText.setText("Connecting...");
    userNameInput.disable();
    connectButton.disable();

    // Tell the sim to initiate the connection.
    uiEventDispatcher.push<ConnectionRequest>({userNameInput.getText()});
}

void TitleWindow::onSimulationStarted()
{
    userInterface.changeScreenTo(
        UserInterfaceExtension::ScreenType::MainScreen);
}

void TitleWindow::onServerConnectionError(ConnectionError connectionError)
{
    // Communicate the connection failure.
    std::string newStatusText{""};
    switch (connectionError.type) {
        case ConnectionError::Type::Disconnected: {
            newStatusText = "Lost connection to server.";
            break;
        }
        case ConnectionError::Type::Failed: {
            newStatusText = "Failed to connect to server.";
            break;
        }
    }
    statusText.setText(newStatusText);

    // Re-enable the interactables.
    userNameInput.enable();
    connectButton.enable();

    // Change back to the title screen (does nothing if we're already there).
    userInterface.changeScreenTo(
        UserInterfaceExtension::ScreenType::TitleScreen);
}

} // End namespace Client
} // End namespace AM
