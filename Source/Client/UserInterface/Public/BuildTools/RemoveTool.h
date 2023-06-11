#pragma once

#include "BuildTool.h"

namespace AM
{
namespace Client 
{

/**
 * The build mode tool used for removing tile layers.
 */
class RemoveTool : public BuildTool
{
public:
    RemoveTool(const World& inWorld, EventDispatcher& inUiEventDispatcher);

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
};

} // End namespace Client
} // End namespace AM
