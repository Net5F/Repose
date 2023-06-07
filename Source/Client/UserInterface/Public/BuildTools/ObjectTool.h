#pragma once

#include "BuildTool.h"

namespace AM
{
struct ObjectSpriteSet;

namespace Client 
{

/**
 *
 */
class ObjectTool : public BuildTool
{
public:
    ObjectTool(EventDispatcher& inUiEventDispatcher);

    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;
    void onMouseLeave() override;

private:
    /** The currently selected sprite set. */
    const ObjectSpriteSet* selectedSpriteSet;
};

} // End namespace Client
} // End namespace AM
