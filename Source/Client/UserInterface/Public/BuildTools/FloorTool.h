#pragma once

#include "BuildTool.h"

namespace AM
{
struct FloorSpriteSet;

namespace Client 
{

/**
 * The build mode tool used for setting and clearing floor tile layers.
 */
class FloorTool : public BuildTool
{
public:
    FloorTool(EventDispatcher& inUiEventDispatcher);

    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected sprite set. */
    const FloorSpriteSet* selectedSpriteSet;
};

} // End namespace Client
} // End namespace AM
