#include "MainButton.h"
#include "Paths.h"

namespace AM
{
namespace Client
{
MainButton::MainButton(const SDL_Rect& inScreenExtent,
                       const std::string& inText,
                       const std::string& inDebugName)
: AUI::Button(inScreenExtent, inDebugName)
{
    // Add our backgrounds.
    normalImage.addResolution({1600, 900},
                              (Paths::TEXTURE_DIR + "MainButton/Normal.png"));
    hoveredImage.addResolution({1600, 900},
                               (Paths::TEXTURE_DIR + "MainButton/Hovered.png"));
    pressedImage.addResolution({1600, 900},
                               (Paths::TEXTURE_DIR + "MainButton/Pressed.png"));

    // Set our text properties.
    text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 21);
    text.setLogicalExtent({0, -1, logicalExtent.w, logicalExtent.h - 1});
    text.setColor({255, 255, 255, 255});
    text.setText(inText);
}

} // End namespace Client
} // End namespace AM
