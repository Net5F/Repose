#pragma once

#include "AUI/Screen.h"
#include "TitleWindow.h"

namespace AM
{
class EventDispatcher;
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
    TitleScreen(UserInterfaceExtension& inUserInterface,
                WorldSinks& inWorldSinks, EventDispatcher& inUiEventDispatcher);

    void render() override;

private:
    //-------------------------------------------------------------------------
    // Windows
    //-------------------------------------------------------------------------
    TitleWindow titleWindow;
};

} // End namespace Client
} // End namespace AM
