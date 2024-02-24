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
    void onMouseUp(AUI::MouseButtonType buttonType,
                   const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** The currently selected graphic set. */
    const FloorGraphicSet* selectedGraphicSet;
};

} // End namespace Client
} // End namespace AM
