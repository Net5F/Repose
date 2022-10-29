#include "TitleScreen.h"
#include "UserInterfaceExtension.h"
#include "AssetCache.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client 
{
TitleScreen::TitleScreen(UserInterfaceExtension& inUserInterface, WorldSinks& inWorldSinks,
                EventDispatcher& inUiEventDispatcher, AssetCache& inAssetCache)
: AUI::Screen("TitleScreen")
, titleWindow{inUserInterface, inWorldSinks, inUiEventDispatcher, inAssetCache}
{
    // Add our windows so they're included in rendering, etc.
    windows.push_back(titleWindow);
}

void TitleScreen::render()
{
    // Fill the background with the background color.
    SDL_Renderer* renderer{AUI::Core::getRenderer()};
    SDL_SetRenderDrawColor(renderer, 37, 37, 52, 255);
    SDL_RenderClear(renderer);

    // Update our window's layouts and render them.
    Screen::render();

    // Set the background back to black in case we switch screens.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

} // End namespace Client
} // End namespace AM
