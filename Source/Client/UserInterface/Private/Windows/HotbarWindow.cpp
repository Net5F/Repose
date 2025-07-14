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
: AUI::Window({0, 0, 1, 1}, "HotbarWindow")
, world{inWorld}
, mainScreen{inMainScreen}
, viewModel{inViewModel}
{
}

AUI::EventResult HotbarWindow::onKeyDown(SDL_Keycode keyCode)
{
    // If the '1' key is pressed, cast a fireball spell.
    if (keyCode == SDLK_1) {
        world.castHelper.queueSpell(
            {.interactionType{SpellType::Fireball},
             .targetEntity{viewModel.getTargetEntity()}});

        return {.wasHandled{true}};
    }

    return {.wasHandled{false}};
}

} // End namespace Client
} // End namespace AM
