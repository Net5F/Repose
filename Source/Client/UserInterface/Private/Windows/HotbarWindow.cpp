#include "HotbarWindow.h"
#include "World.h"
#include "MainScreen.h"
#include "ViewModel.h"
#include "CastRequest.h"

namespace AM
{
namespace Client
{
HotbarWindow::HotbarWindow(World& inWorld, MainScreen& inMainScreen,
                           const ViewModel& inViewModel)
: AUI::Window({1362, 340, 256, 256}, "HotbarWindow")
, world{inWorld}
, mainScreen{inMainScreen}
, viewModel{inViewModel}
{
}

AUI::EventResult HotbarWindow::onKeyDown(SDL_Keycode keyCode)
{
    // If the '1' key is pressed, cast a fireball spell.
    if (keyCode == SDLK_1) {
        CastFailureType result{world.castHelper.castSpell(
            {.interactionType{SpellType::Fireball},
             .targetEntity{viewModel.getTargetEntity()}})};
        std::string_view resultString{getCastFailureString(result)};
        if (resultString.compare("") != 0) {
            mainScreen.addChatMessage(resultString);
        }

        return {.wasHandled{true}};
    }

    return {.wasHandled{false}};
}

} // End namespace Client
} // End namespace AM
