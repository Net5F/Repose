#include "TitleTextInput.h"
#include "Paths.h"

namespace AM
{
namespace Client
{
TitleTextInput::TitleTextInput(const SDL_Rect& inScreenExtent,
                               const std::string& inDebugName)
: AUI::TextInput(inScreenExtent, inDebugName)
{
    // Add our backgrounds.
    normalImage.setSimpleImage(Paths::TEXTURE_DIR + "TextInput/Normal.png");
    hoveredImage.setSimpleImage(Paths::TEXTURE_DIR + "TextInput/Hovered.png");
    focusedImage.setSimpleImage(Paths::TEXTURE_DIR + "TextInput/Selected.png");
    disabledImage.setSimpleImage(Paths::TEXTURE_DIR + "TextInput/Disabled.png");

    // Set our text properties.
    setTextFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 25);

    // Set our input box properties.
    setCursorWidth(2);
}

} // End namespace Client
} // End namespace AM
