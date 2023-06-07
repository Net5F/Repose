#include "WallTool.h"

namespace AM
{
namespace Client 
{

WallTool::WallTool(EventDispatcher& inUiEventDispatcher)
: BuildTool(inUiEventDispatcher)
, selectedSpriteSet{nullptr}
{
}

void WallTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only floor covering sprite sets 
    //       should be clickable while this tool is alive.
    selectedSpriteSet
        = static_cast<const WallSpriteSet*>(&inSelectedSpriteSet);
}

void WallTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
}

void WallTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
}

void WallTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void WallTool::onMouseWheel(int amountScrolled)
{
}

void WallTool::onMouseMove(const SDL_Point& cursorPosition)
{
}

void WallTool::onMouseLeave()
{
}

} // End namespace Client
} // End namespace AM
