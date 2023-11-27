#pragma once

#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/VerticalListContainer.h"
#include <functional>

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
class RightClickMenu : public AUI::Window
{
public:
    /** The padding to add on each side of this menu. Allows the mouse to move 
        slightly outside the menu without accidentally closing it. */
    static constexpr int PADDING{20};

    RightClickMenu();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The menu's background */
    AUI::Image backgroundImage;

    //-------------------------------------------------------------------------
    // Limited public interface of private widgets
    //-------------------------------------------------------------------------
    /**
     * Adds an action to this menu.
     * 
     * @param displayText The text to display for this menu option.
     * @param onSelected The function to call when this option is selected.
     */
    void addMenuAction(std::string_view displayText,
                       std::function<void(void)> onSelected);

    /**
     * Clears all actions from this menu.
     */
    void clear();

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void onMouseLeave() override;

private:
    /** The base menu width, before padding is added. Includes border width. */
    static constexpr int MENU_WIDTH{169};

    /** The logical width of the border in the background image. */
    static constexpr int BORDER_WIDTH{5};

    /** The logical height of each action button in the menu. */
    static constexpr int BUTTON_HEIGHT{32};

    /**
     * Styles the given button and sets its text to the given text.
     */
    void styleButton(AUI::Button& button, std::string_view text);

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    /** The menu's selectable actions. */
    AUI::VerticalListContainer actionContainer;
};

} // End namespace Client
} // End namespace AM
