#pragma once

#include "BuildTool.h"

namespace AM
{
namespace Client 
{
class WorldObjectLocator;

/**
 * The build mode tool used for removing tile layers.
 */
class RemoveTool : public BuildTool
{
public:
    RemoveTool(const World& inWorld,
               const WorldObjectLocator& inWorldObjectLocator,
               EventDispatcher& inUiEventDispatcher);

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** Used for finding tile layers or entities that the mouse is 
        hovering over or clicking. */
    const WorldObjectLocator& worldObjectLocator;
};

} // End namespace Client
} // End namespace AM
