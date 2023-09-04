#include "UserInterfaceExtension.h"
#include "Config.h"
#include "World.h"
#include "WorldSinks.h"
#include "Network.h"
#include "AssetCache.h"
#include "SpriteData.h"
#include "Camera.h"
#include "SharedConfig.h"
#include "UserConfig.h"
#include "Transforms.h"
#include "ClientTransforms.h"
#include "Log.h"
#include "AUI/Core.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Client
{

UserInterfaceExtension::UserInterfaceExtension(UserInterfaceExDependencies deps)
: worldSinks{deps.worldSignals}
, auiInitializer{deps.sdlRenderer,
                 {Config::LOGICAL_SCREEN_WIDTH, Config::LOGICAL_SCREEN_HEIGHT}}
, titleScreen{*this, worldSinks, deps.uiEventDispatcher}
, mainScreen{deps.world, worldSinks, deps.worldObjectLocator,
             deps.uiEventDispatcher, deps.network, deps.spriteData}
, currentScreen{&titleScreen}
, userErrorStringQueue{deps.network.getEventDispatcher()}
{
    SDL_Rect windowSize{UserConfig::get().getWindowSize()};
    AUI::Core::setActualScreenSize({windowSize.w, windowSize.h});
}

void UserInterfaceExtension::changeScreenTo(ScreenType screenType)
{
    switch (screenType) {
        case ScreenType::TitleScreen: {
            currentScreen = &titleScreen;
            break;
        }
        case ScreenType::MainScreen: {
            currentScreen = &mainScreen;
            break;
        }
        default: {
            currentScreen = &titleScreen;
            break;
        }
    }
}

std::vector<PhantomSpriteInfo> UserInterfaceExtension::getPhantomSprites() const
{
    if (currentScreen == &mainScreen) {
        return mainScreen.getPhantomSprites();
    }
    else {
        return {};
    }
}

std::vector<SpriteColorModInfo>
    UserInterfaceExtension::getSpriteColorMods() const
{
    if (currentScreen == &mainScreen) {
        return mainScreen.getSpriteColorMods();
    }
    else {
        return {};
    }
}

bool UserInterfaceExtension::handleOSEvent(SDL_Event& event)
{
    return currentScreen->handleOSEvent(event);
}

void UserInterfaceExtension::tick(double timestepS)
{
    currentScreen->tick(timestepS);

    // TEMP: Process any waiting error strings.
    UserErrorString userErrorString{};
    while (userErrorStringQueue.pop(userErrorString)) {
        LOG_INFO("Server: %s", userErrorString.errorString.c_str());
    }
}

void UserInterfaceExtension::render(const Camera& camera)
{
    mainScreen.setCamera(camera);

    currentScreen->render();
}

} // End namespace Client
} // End namespace AM
