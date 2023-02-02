#pragma once

#include "AUI/Window.h"
#include "AUI/Text.h"

namespace AM
{
namespace Client
{
class WorldSinks;

/**
 * The main overlay on the main screen. Shows basic HUD information.
 */
class MainOverlay : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    MainOverlay();

    /**
     * Sets the visibility of the hint text.
     * Used by the MainScreen to control the hint text, so we don't have to
     * do "is the player in the build area" checks in two places.
     */
    void setBuildModeHintVisibility(bool isVisible);

private:
    /** The "Press 'b' to enter Build Mode" text. */
    AUI::Text buildModeHintText;
};

} // End namespace Client
} // End namespace AM
