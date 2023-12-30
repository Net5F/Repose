#include "RightClickMenu.h"
#include "MainScreen.h"
#include "Paths.h"

namespace AM
{
namespace Client
{

/** The default height. This should never be seen since we adjust to fit the
    content, but we need a default for textures. */
static constexpr int DEFAULT_HEIGHT{150};

RightClickMenu::RightClickMenu()
: AUI::Window(
    {0, 0, MENU_WIDTH + (PADDING * 2), DEFAULT_HEIGHT + (PADDING * 2)},
    "RightClickMenu")
, backgroundImage({PADDING, PADDING, MENU_WIDTH, DEFAULT_HEIGHT})
, actionContainer({(PADDING + BORDER_WIDTH), (PADDING + BORDER_WIDTH),
                   (MENU_WIDTH - (BORDER_WIDTH * 2)),
                   (DEFAULT_HEIGHT - (BORDER_WIDTH * 2))},
                  "ActionContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(actionContainer);

    /* Background image. */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});

    /* Container. */
    actionContainer.setGapSize(1);
}

void RightClickMenu::addMenuAction(std::string_view displayText,
                                   std::function<void(void)> onSelected)
{
    // Add the new action.
    std::unique_ptr<AUI::Widget> newMenuAction{std::make_unique<AUI::Button>(
        SDL_Rect{0, 0, actionContainer.getLogicalExtent().w, BUTTON_HEIGHT},
        "RightClickMenuButton")};
    AUI::Button& button{static_cast<AUI::Button&>(*newMenuAction)};
    styleButton(button, displayText);
    button.setOnPressed(std::move(onSelected));

    actionContainer.push_back(std::move(newMenuAction));

    // Grow this menu's height to fit the new action.
    int contentHeight{BUTTON_HEIGHT * static_cast<int>(actionContainer.size())};
    SDL_Rect newMenuExtent{logicalExtent};
    newMenuExtent.h = (PADDING * 2) + (BORDER_WIDTH * 2) + contentHeight;
    setLogicalExtent(newMenuExtent);

    SDL_Rect newBackgroundExtent{backgroundImage.getLogicalExtent()};
    newBackgroundExtent.h = contentHeight + (BORDER_WIDTH * 2);
    backgroundImage.setLogicalExtent(newBackgroundExtent);

    SDL_Rect newContainerExtent{actionContainer.getLogicalExtent()};
    newContainerExtent.h = contentHeight;
    actionContainer.setLogicalExtent(newContainerExtent);
}

void RightClickMenu::clear()
{
    actionContainer.clear();
}

void RightClickMenu::onMouseLeave()
{
    // When the mouse leaves our padded bounds, close this menu.
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
