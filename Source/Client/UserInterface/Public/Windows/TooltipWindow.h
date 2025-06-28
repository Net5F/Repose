#pragma once

#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/Text.h"

namespace AUI
{
class Button;
}

namespace AM
{
namespace Client
{

/**
 * A general-purpose menu that displays a custom list of selectable actions.
 */
class TooltipWindow : public AUI::Window
{
public:
    TooltipWindow();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The menu's background */
    AUI::Image backgroundImage;

    /** The tooltip text. */
    AUI::Text text;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void setIsVisible(bool inIsVisible) override;

    void onTick(double timestepS) override;

    void measure() override;

private:
    /**
     * Updates this window's position to match the current cursor position.
     */
    void updatePosition();

    /** The logical width of the border in the background image. */
    static constexpr int BORDER_WIDTH{8};

    /** The logical Y-axis distance between the cursor and this tooltip. */
    static constexpr int CURSOR_VERTICAL_OFFSET{55};
};

} // End namespace Client
} // End namespace AM
