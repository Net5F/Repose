#pragma once

#include "AUI/Window.h"

namespace AM
{
namespace Client
{
class World;
class MainScreen;
class ViewModel;

/**
 * The hotbar that manages castable->keyboard button assignments.
 *
 * TODO: For now, there's no UI. This is just used as a place to handle 
 *       button presses.
 */
class HotbarWindow : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    HotbarWindow(World& inWorld, MainScreen& inMainScreen,
                 const ViewModel& inViewModel);

    // Note: Since this widget doesn't take focus, this function won't be 
    //       called automatically. Instead, MainScreen has to manually call 
    //       this function when an input is unhandled by everything else.
    AUI::EventResult onKeyDown(SDL_Keycode keyCode);

private:
    /** Used to request casts. */
    World& world;
    /** Used to print cast failure messages to the chat. */
    MainScreen& mainScreen;
    /** Used to get the currently targeted entity. */
    const ViewModel& viewModel;
};

} // End namespace Client
} // End namespace AM
