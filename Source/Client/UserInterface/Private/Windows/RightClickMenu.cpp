#include "RightClickMenu.h"
#include "MainScreen.h"
#include "Paths.h"
#include "Ignore.h"

namespace AM
{
namespace Client
{
// TODO: Size this to fit the max number of interactions
//       Fix highlight sizing to not overlap border
RightClickMenu::RightClickMenu()
: AUI::Window({0, 0, 169, 187}, "RightClickMenu")
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h})
, actionContainer({0, 0, logicalExtent.w, logicalExtent.h}, "ActionContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(actionContainer);

    // Flag ourselves as focusable, so we can close when focus is lost.
    isFocusable = true;

    /* Background image. */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});

    /* Container. */
    actionContainer.setGapSize(1);
}

void RightClickMenu::addMenuAction(std::string_view displayText,
                                   std::function<void(void)> onSelected)
{
    std::unique_ptr<AUI::Widget> newMenuAction{std::make_unique<AUI::Button>(
        SDL_Rect{1, 1, (logicalExtent.w - 2), 32}, "RightClickMenuButton")};
    AUI::Button& button{static_cast<AUI::Button&>(*newMenuAction)};
    styleButton(button, displayText);
    button.setOnPressed(std::move(onSelected));

    actionContainer.push_back(std::move(newMenuAction));
}

void RightClickMenu::onFocusLost(AUI::FocusLostType focusLostType)
{
    // When we lose focus, close the menu.
    setIsVisible(false);
}

void RightClickMenu::styleButton(AUI::Button& button, std::string_view text)
{
    button.text.setLogicalExtent({10, 0, (logicalExtent.w - 20), 31});
    button.hoveredImage.setSimpleImage(Paths::TEXTURE_DIR
                                       + "Highlights/Hovered.png");
    button.text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 18);
    button.text.setColor({255, 255, 255, 255});
    button.text.setText(text);
    button.text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Left);
}

} // End namespace Client
} // End namespace AM
