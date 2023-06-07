#include "FloorCoveringTool.h"

namespace AM
{
namespace Client
{

FloorCoveringTool::FloorCoveringTool(EventDispatcher& inUiEventDispatcher)
: BuildTool(inUiEventDispatcher)
, selectedSpriteSet{nullptr}
{
}

void FloorCoveringTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only floor covering sprite sets 
    //       should be clickable while this tool is alive.
    selectedSpriteSet
        = static_cast<const FloorCoveringSpriteSet*>(&inSelectedSpriteSet);
}

void FloorCoveringTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
}

void FloorCoveringTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
}

void FloorCoveringTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void FloorCoveringTool::onMouseWheel(int amountScrolled)
{
}

void FloorCoveringTool::onMouseMove(const SDL_Point& cursorPosition)
{
}

void FloorCoveringTool::onMouseLeave()
{
}

} // End namespace Client
} // End namespace AM
