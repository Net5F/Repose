#pragma once

#include "AUI/Window.h"

namespace AM
{
class CastableData;

namespace Client
{
class Network;

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
    HotbarWindow(CastableData& inCastableData, Network& inNetwork);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    bool onKeyDown(SDL_Keycode keyCode) override;

private:
    /** Used to get Castable definitions. */
    CastableData& castableData;
    /** Used to send cast requests. */
    Network& network;
};

} // End namespace Client
} // End namespace AM
