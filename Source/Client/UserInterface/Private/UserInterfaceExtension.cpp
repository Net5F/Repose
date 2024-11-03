#include "UserInterfaceExtension.h"
#include "UserInterfaceExDependencies.h"
#include "Config.h"
#include "World.h"
#include "Network.h"
#include "AssetCache.h"
#include "Camera.h"
#include "SharedConfig.h"
#include "UserConfig.h"
#include "Transforms.h"
#include "ClientTransforms.h"
#include "Log.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client
{

UserInterfaceExtension::UserInterfaceExtension(
    const UserInterfaceExDependencies& deps)
: auiInitializer{deps.sdlRenderer,
                 {Config::LOGICAL_SCREEN_WIDTH, Config::LOGICAL_SCREEN_HEIGHT},
                 {UserConfig::get().getWindowSize().w,
                  UserConfig::get().getWindowSize().h}}
, titleScreen{*this, deps.simulation, deps.uiEventDispatcher}
, mainScreen{deps}
, currentScreen{&titleScreen}
{
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
}

void UserInterfaceExtension::render(const Camera& camera)
{
    mainScreen.setCamera(camera);

    currentScreen->render();
}

} // End namespace Client
} // End namespace AM
