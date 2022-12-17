#include "MainOverlay.h"
#include "Paths.h"
#include "Ignore.h"

namespace AM
{
namespace Client
{
MainOverlay::MainOverlay()
: AUI::Window({0, 0, 1920, 1080}, "MainOverlay")
, buildModeHintText({50, 850, 500, 100}, "BuildModeHintText")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(buildModeHintText);

    /* Build mode text. */
    buildModeHintText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    buildModeHintText.setColor({255, 255, 255, 255});
    buildModeHintText.setText("Press 'b' to enter Build Mode");
    buildModeHintText.setHorizontalAlignment(
        AUI::Text::HorizontalAlignment::Left);
    buildModeHintText.setIsVisible(false);
}

void MainOverlay::setBuildModeHintVisibility(bool isVisible)
{
    buildModeHintText.setIsVisible(isVisible);
}

} // End namespace Client
} // End namespace AM
