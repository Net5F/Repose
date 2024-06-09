#pragma once

#include "BuildTool.h"
#include "TilePosition.h"

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

    /** The indices within selectedGraphicSet->graphics that contain a 
        graphic. */
    std::vector<std::size_t> validGraphicIndices;

    /** The index within validGraphicIndices that is currently selected. */
    std::size_t selectedGraphicIndex;
};

} // End namespace Client
} // End namespace AM
