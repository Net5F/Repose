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
    RightClickMenu();

    /**
     * Adds an action to this menu.
     * 
     * @param displayText The text to display for this menu option.
     * @param onSelected The function to call when this option is selected.
     */
    void addMenuAction(std::string_view displayText,
                       std::function<void(void)> onSelected);

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The menu's background */
    AUI::Image backgroundImage;

    /** The menu's selectable actions. */
    AUI::VerticalListContainer actionContainer;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void onFocusLost(AUI::FocusLostType focusLostType) override;

private:
    /**
     * Styles the given button and sets its text to the given text.
     */
    void styleButton(AUI::Button& button, std::string_view text);
};

} // End namespace Client
} // End namespace AM
