#pragma once

#include "BuildTool.h"

namespace AM
{
struct ObjectSpriteSet;

namespace Client 
{
class WorldObjectLocator;

/**
 * The build mode tool used for adding entities.
 */
class EntityTool : public BuildTool
{
public:
    EntityTool(const World& inWorld,
               const WorldObjectLocator& inWorldObjectLocator,
               EventDispatcher& inUiEventDispatcher);

    /**
     * Sets the currently selected entity. This selection will follow the 
     * user's mouse, and will be placed if the user left clicks.
     * 
     * Note: This tool has no concept of "templates". The content panel just 
     *       gives us the appropriate data to display and send.
     */
    void setSelectedEntity(const std::string& name,
                                  const ObjectSpriteSet& spriteSet,
                                  Uint8 defaultSpriteIndex);

    void onMouseDown(AUI::MouseButtonType buttonType,
                     const SDL_Point& cursorPosition) override;
    void onMouseDoubleClick(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;
    void onMouseWheel(int amountScrolled) override;
    void onMouseMove(const SDL_Point& cursorPosition) override;

private:
    /** Used for finding entities that the mouse is hovering over or 
        clicking. */
    const WorldObjectLocator& worldObjectLocator;

    /** The name of the currently selected dynamic object. If empty, there's 
        no current selection. */
    std::string selectedObjectName;

    /** The selected object's sprite set. If nullptr, there's no current 
        selection. */
    const ObjectSpriteSet* selectedSpriteSet;

    /** The indices within selectedSpriteSet->sprites that contain a sprite. */
    std::vector<std::size_t> validSpriteIndices;

    /** The index within validSpriteIndices that is currently selected. */
    std::size_t selectedSpriteIndex;
};

} // End namespace Client
} // End namespace AM
