#include "HotbarWindow.h"
#include "CastableData.h"
#include "Network.h"
#include "CastRequest.h"

namespace AM
{
namespace Client
{
HotbarWindow::HotbarWindow(CastableData& inCastableData, Network& inNetwork)
: AUI::Window({1362, 340, 256, 256}, "HotbarWindow")
, castableData{inCastableData}
, network{inNetwork}
{
}

bool HotbarWindow::onKeyDown(SDL_Keycode keyCode)
{
    // TODO: Need target entity
    // If the '1' key is pressed, cast a fireball spell.
    if (keyCode == SDLK_1) {
        const Castable& fireball{castableData.getCastable(SpellType::Fireball)};
        network.serializeAndSend(CastRequest{fireball->castableID, 0,
                                             params.targetEntity});
    }

    return false;
}

} // End namespace Client
} // End namespace AM
