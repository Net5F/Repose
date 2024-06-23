#pragma once

#include "BuildTool.h"

namespace AM
{
struct FloorGraphicSet;

namespace Client
{

/**
 * The build mode tool used for setting floor tile layers.
 */
class FloorTool : public BuildTool
{
public:
    FloorTool(World& inWorld, Network& inNetwork);

    void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected graphic set. */
    const FloorGraphicSet* selectedGraphicSet;

    /** The indices within selectedGraphicSet->graphics that contain a 
        graphic. */
    std::vector<std::size_t> validGraphicIndices;

    /** The index within validGraphicIndices that is currently selected. */
    std::size_t selectedGraphicIndex;

    /** The currently selected tile offset. */
    TileOffset selectedTileOffset;

    /** If true, the user is currently adjusting the X/Y offset. */
    bool adjustingXYOffset;

    /** If adjustingXYOffset == true, this is the position where the adjustment 
        started. */
    Position xyOffsetAdjustmentOrigin;
};

} // End namespace Client
} // End namespace AM
