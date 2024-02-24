#pragma once

#include "BuildTool.h"
#include "TilePosition.h"

namespace AM
{
struct FloorCoveringGraphicSet;

namespace Client
{

/**
 * The build mode tool used for adding floor covering tile layers.
 */
class FloorCoveringTool : public BuildTool
{
public:
    FloorCoveringTool(World& inWorld, Network& inNetwork);

    void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected graphic set. */
    const FloorCoveringGraphicSet* selectedGraphicSet;

    /** The indices within selectedGraphicSet->graphic that contain a graphic. */
    std::vector<std::size_t> validGraphicIndices;

    /** The index within validGraphicIndices that is currently selected. */
    std::size_t selectedGraphicIndex;
};

} // End namespace Client
} // End namespace AM
