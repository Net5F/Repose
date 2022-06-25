#include "UserInterfaceExtension.h"
#include "Config.h"
#include "WorldSinks.h"
#include "AssetCache.h"
#include "SpriteData.h"
#include "Camera.h"
#include "SharedConfig.h"
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
, mainScreen{worldSinks, deps.uiEventDispatcher, deps.assetCache, deps.spriteData}
, currentScreen{&mainScreen}
{
    AUI::Core::setActualScreenSize(
        {Config::ACTUAL_SCREEN_WIDTH, Config::ACTUAL_SCREEN_HEIGHT});
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
