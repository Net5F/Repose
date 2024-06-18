#pragma once

#include "BuildTool.h"
#include "TilePosition.h"
#include "Terrain.h"

namespace AM
{
struct TerrainGraphicSet;

namespace Client
{

/**
 * The build mode tool used for adding terrain tile layers.
 */
class TerrainTool : public BuildTool
{
public:
    TerrainTool(World& inWorld, Network& inNetwork);

    void setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet) override;

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected graphic set. */
    const TerrainGraphicSet* selectedGraphicSet;

    /** The terrain heights within selectedGraphicSet->graphics that contain a 
        graphic. */
    std::vector<std::size_t> validHeights;

    /** The index within validHeightIndices that is currently selected. */
    std::size_t selectedHeightIndex;

    /** The current terrain start height. */
    Terrain::Height selectedStartHeight;
};

} // End namespace Client
} // End namespace AM
