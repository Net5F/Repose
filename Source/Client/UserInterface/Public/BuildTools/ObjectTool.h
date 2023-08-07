#pragma once

#include "BuildTool.h"

namespace AM
{
struct ObjectSpriteSet;

namespace Client 
{

/**
 * The build mode tool used for adding object tile layers.
 */
class ObjectTool : public BuildTool
{
public:
    ObjectTool(const World& inWorld, Network& inNetwork);

    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected sprite set. */
    const ObjectSpriteSet* selectedSpriteSet;

    /** The indices within selectedSpriteSet->sprites that contain a sprite. */
    std::vector<std::size_t> validSpriteIndices;

    /** The index within validSpriteIndices that is currently selected. */
    std::size_t selectedSpriteIndex;
};

} // End namespace Client
} // End namespace AM
