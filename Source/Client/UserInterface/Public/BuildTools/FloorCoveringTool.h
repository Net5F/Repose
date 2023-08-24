#pragma once

#include "BuildTool.h"
#include "TilePosition.h"

namespace AM
{
struct FloorCoveringSpriteSet;

namespace Client 
{

/**
 * The build mode tool used for adding floor covering tile layers.
 */
class FloorCoveringTool : public BuildTool
{
public:
    FloorCoveringTool(World& inWorld, Network& inNetwork);

    void setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected sprite set. */
    const FloorCoveringSpriteSet* selectedSpriteSet;

    /** The indices within selectedSpriteSet->sprites that contain a sprite. */
    std::vector<std::size_t> validSpriteIndices;

    /** The index within validSpriteIndices that is currently selected. */
    std::size_t selectedSpriteIndex;
};

} // End namespace Client
} // End namespace AM
