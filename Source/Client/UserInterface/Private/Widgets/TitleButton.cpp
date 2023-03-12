#include "TitleButton.h"
#include "Paths.h"

namespace AM
{
namespace Client
{
TitleButton::TitleButton(const SDL_Rect& inScreenExtent,
                         const std::string& inText,
                         const std::string& inDebugName)
: AUI::Button(inScreenExtent, inDebugName)
{
    // Add our backgrounds.
    normalImage.addResolution({1920, 1080},
                              (Paths::TEXTURE_DIR + "TitleButton/Normal.png"));
    hoveredImage.addResolution(
        {1920, 1080}, (Paths::TEXTURE_DIR + "TitleButton/Hovered.png"));
    pressedImage.addResolution(
        {1920, 1080}, (Paths::TEXTURE_DIR + "TitleButton/Pressed.png"));
    disabledImage.addResolution(
        {1920, 1080}, (Paths::TEXTURE_DIR + "TitleButton/Disabled.png"));

    // Set our text properties.
    text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 33);
    text.setColor({255, 255, 255, 255});
    text.setText(inText);
}

} // End namespace Client
} // End namespace AM
