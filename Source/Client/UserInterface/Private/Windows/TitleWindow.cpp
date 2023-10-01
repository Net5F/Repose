#include "TitleWindow.h"
#include "UserInterfaceExtension.h"
#include "Simulation.h"
#include "TitleScreen.h"
#include "ConnectionRequest.h"
#include "Paths.h"
#include "Log.h"

namespace AM
{
namespace Client
{
TitleWindow::TitleWindow(UserInterfaceExtension& inUserInterface,
                         Simulation& inSimulation,
                         EventDispatcher& inUiEventDispatcher)
: AUI::Window({0, 0, 1920, 1080}, "TitleWindow")
, userInterface{inUserInterface}
, uiEventDispatcher{inUiEventDispatcher}
, backgroundImage{{0, 0, 1920, 1080}, "TitleBackground"}
, leadText({0, 266, 1920, 300}, "LeadText")
, titleText({0, 248, 1920, 300}, "TitleText")
//, userNameLabel({0, 435, 1920, 36}, "UserNameLabel")
//, userNameInput(assetCache, {770, 481, 380, 54}, "UserNameInput")
, connectButton({760, 518, 404, 70}, "Connect", "ConnectButton")
, statusText({0, 618, 1920, 48}, "StatusText")
, randGenerator{std::random_device()()}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(leadText);
    children.push_back(titleText);
    // children.push_back(userNameLabel);
    // children.push_back(userNameInput);
    children.push_back(connectButton);
    children.push_back(statusText);

    /* Background image */
    backgroundImage.setMultiResImage(
        {{{1920, 1080},
          (Paths::TEXTURE_DIR + "TitleBackground/Background_1920.png")},
         {{1600, 900},
          (Paths::TEXTURE_DIR + "TitleBackground/Background_1600.png")},
         {{1280, 720},
          (Paths::TEXTURE_DIR + "TitleBackground/Background_1280.png")}});

    /* Lead text. */
    leadText.setRenderMode(AUI::Text::RenderMode::Blended);
    leadText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    leadText.setColor({255, 255, 255, 255});
    leadText.setText(getRandomLeadText());
    leadText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    /* Title text. */
    titleText.setRenderMode(AUI::Text::RenderMode::Blended);
    titleText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 185);
    titleText.setColor({255, 255, 255, 255});
    titleText.setText("Repose");
    titleText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    /* User name entry. */
    // userNameLabel.setFont((Paths::FONT_DIR + "B612-Regular.ttf"), 30);
    // userNameLabel.setColor({255, 255, 255, 255});
    // userNameLabel.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    // userNameLabel.setText("User Name");

    // userNameInput.setTextFont((Paths::FONT_DIR + "B612-Regular.ttf"), 28);
    // userNameInput.setMargins({12, 0, 12, 0});

    /* Status text. */
    statusText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    statusText.setColor({255, 255, 255, 255});
    statusText.setText("");
    statusText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    statusText.setIsVisible(true);

    // Register our UI event handlers.
    connectButton.setOnPressed(
        std::bind(&TitleWindow::onConnectButtonPressed, this));

    // Register our world signal handlers.
    inSimulation.simulationStarted.connect<&TitleWindow::onSimulationStarted>(
        *this);
    inSimulation.serverConnectionError
        .connect<&TitleWindow::onServerConnectionError>(*this);
}

void TitleWindow::onConnectButtonPressed()
{
    // Communicate that we're attempting to connect and disable interactables.
    statusText.setText("Connecting...");
    // userNameInput.disable();
    connectButton.disable();

    // Tell the sim to initiate the connection.
    // uiEventDispatcher.push<ConnectionRequest>({userNameInput.getText()});
    uiEventDispatcher.push<ConnectionRequest>({""});
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
    // userNameInput.enable();
    connectButton.enable();

    // Change back to the title screen (does nothing if we're already there).
    userInterface.changeScreenTo(
        UserInterfaceExtension::ScreenType::TitleScreen);
}

std::string TitleWindow::getRandomLeadText()
{
    std::vector<std::string> leadTextStrings;
    leadTextStrings.push_back("Welcome to the world of");
    leadTextStrings.push_back("The ever-changing world of");
    leadTextStrings.push_back("The warm sun shines on");
    leadTextStrings.push_back("The flowers are blooming in");

    std::uniform_int_distribution<std::size_t> dist{
        0, (leadTextStrings.size() - 1)};
    return leadTextStrings[dist(randGenerator)];
}

} // End namespace Client
} // End namespace AM
