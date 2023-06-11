#include "RemoveTool.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "Ignore.h"

namespace AM
{
namespace Client 
{
RemoveTool::RemoveTool(const World& inWorld, EventDispatcher& inUiEventDispatcher)
: BuildTool(inWorld, inUiEventDispatcher)
{
}

void RemoveTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().
    ignore(cursorPosition);

    // If this tool is active, the user left clicked, and we have a selected 
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
    }
}

void RemoveTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
    // TODO: Add support for click-and-drag to remove an extent instead of a 
    //       single layer.
}

void RemoveTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void RemoveTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // If this tool is active.
    if (isActive) {
    }
}

} // End namespace Client
} // End namespace AM
