#pragma once

#include "BuildTool.h"

namespace AM
{
struct ObjectSpriteSet;

namespace Client 
{
class WorldObjectLocator;

/**
 * The build mode tool used for adding object entities (dynamic objects).
 */
class DynamicObjectTool : public BuildTool
{
public:
    DynamicObjectTool(const World& inWorld,
               const WorldObjectLocator& inWorldObjectLocator,
               Network& inNetwork);

    /**
     * Sets the currently selected dynamic object. This selection will follow 
     * the user's mouse, and will be placed if the user left clicks.
     * 
     * Note: This tool has no concept of "templates". The content panel just 
     *       gives us the appropriate data to display and send.
     */
    void setSelectedObject(const std::string& name, const Rotation& rotation,
                           const ObjectSpriteSet& spriteSet);

    /**
     * @param inOnSelectionCleared  A callback for when the user right clicks 
     *                              or presses escape to clear the selection.
     */
    void setOnSelectionCleared(std::function<void(void)> inOnSelectionCleared);

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /**
     * If we have a curent selection, clears it and calls onSelectionCleared.
     */
    void clearCurrentSelection();

    /** Used for finding objects that the mouse is hovering over or 
        clicking. */
    const WorldObjectLocator& worldObjectLocator;

    /** The color used to highlight the hovered object. */
    const SDL_Color highlightColor;

    /** The name of the currently selected object. If empty, there's no 
        current selection. */
    std::string selectedObjectName;

    /** The selected object's sprite set. If nullptr, there's no current 
        selection. */
    const ObjectSpriteSet* selectedSpriteSet;

    /** The indices within selectedSpriteSet->sprites that contain a sprite. */
    std::vector<std::size_t> validSpriteIndices;

    /** The index within validSpriteIndices that is currently selected. */
    std::size_t selectedSpriteIndex;

    std::function<void(void)> onSelectionCleared;
};

} // End namespace Client
} // End namespace AM
