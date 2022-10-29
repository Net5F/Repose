#pragma once

#include "AUI/Screen.h"
#include "TitleWindow.h"

namespace AM
{
class EventDispatcher;
class AssetCache;

namespace Client
{
class UserInterfaceExtension;
class WorldSinks;

/**
 * The opening title screen that you see on app launch.
 */
class TitleScreen : public AUI::Screen
{
public:
    TitleScreen(UserInterfaceExtension& inUserInterface, WorldSinks& inWorldSinks,
                EventDispatcher& inUiEventDispatcher, AssetCache& inAssetCache);

    void render() override;

private:
    //-------------------------------------------------------------------------
    // Windows
    //-------------------------------------------------------------------------
    TitleWindow titleWindow;
};

} // End namespace Client
} // End namespace AM
