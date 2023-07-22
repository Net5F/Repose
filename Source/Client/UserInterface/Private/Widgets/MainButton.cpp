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
    normalImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "MainButton/Normal.png"), {2, 3, 4, 2});
    hoveredImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "MainButton/Hovered.png"), {2, 3, 4, 2});
    pressedImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "MainButton/Pressed.png"), {2, 3, 4, 2});

    // Set our text properties.
    text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
    text.setLogicalExtent({0, -1, logicalExtent.w, logicalExtent.h - 1});
    text.setColor({255, 255, 255, 255});
    text.setText(inText);
}

} // End namespace Client
} // End namespace AM
