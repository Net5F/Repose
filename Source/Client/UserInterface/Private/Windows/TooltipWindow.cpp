#include "TooltipWindow.h"
#include "MainScreen.h"
#include "Paths.h"
#include "AUI/ScalingHelpers.h"

namespace AM
{
namespace Client
{

TooltipWindow::TooltipWindow()
: AUI::Window({0, 0, 400, 40}, "TooltipWindow")
, backgroundImage(logicalExtent, "TooltipBackground")
, text({BORDER_WIDTH, BORDER_WIDTH, (logicalExtent.w - (BORDER_WIDTH * 2)),
        (logicalExtent.h - (BORDER_WIDTH * 2))},
       "TooltipText")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(text);

    /* Background image. */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
    backgroundImage.setAlphaMod(0.6f);

    /* Text. */
    text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 18);
    text.setColor({255, 255, 255, 255});
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Left);
    text.setText("");
}

void TooltipWindow::setIsVisible(bool inIsVisible)
{
    // Call the normal setter.
    Widget::setIsVisible(inIsVisible);

    // We need to immediately update our position because we'll be rendered 
    // once before onTick() is called.
    updatePosition();
}

void TooltipWindow::onTick(double)
{
    // Update our position to constantly follow the cursor.
    updatePosition();
}

void TooltipWindow::measure()
{
    // Do the normal measure step (text will resize itself).
    Window::measure();

    // Resize the background to match the text's texture.
    SDL_Rect textTextureExtent{text.getLogicalTextureExtent()};
    backgroundImage.setLogicalExtent(
        {0, 0, textTextureExtent.w + (BORDER_WIDTH * 2),
         textTextureExtent.h + (BORDER_WIDTH * 2)});

    // Measure again since we changed backgroundImage.
    Window::measure();
}

void TooltipWindow::updatePosition()
{
    // Get the current mouse position and convert to logical.
    SDL_Point cursorPosition{};
    SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
    cursorPosition = AUI::ScalingHelpers::actualToLogical(cursorPosition);

    // Place this window right below the cursor.
    SDL_Rect newExtent{logicalExtent};
    newExtent.x = cursorPosition.x;
    newExtent.y = (cursorPosition.y + CURSOR_VERTICAL_OFFSET);
    setLogicalExtent(newExtent);
}

} // End namespace Client
} // End namespace AM
