#include "ObjectTool.h"

namespace AM
{
namespace Client 
{

ObjectTool::ObjectTool(EventDispatcher& inUiEventDispatcher)
: BuildTool(inUiEventDispatcher)
, selectedSpriteSet{nullptr}
{
}

void ObjectTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only floor covering sprite sets 
    //       should be clickable while this tool is alive.
    selectedSpriteSet
        = static_cast<const ObjectSpriteSet*>(&inSelectedSpriteSet);
}

void ObjectTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
}

void ObjectTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
}

void ObjectTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void ObjectTool::onMouseWheel(int amountScrolled)
{
}

void ObjectTool::onMouseMove(const SDL_Point& cursorPosition)
{
}

void ObjectTool::onMouseLeave()
{
}

} // End namespace Client
} // End namespace AM
