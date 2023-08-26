#pragma once

#include "BuildTool.h"
#include "InitScriptResponse.h"
#include "QueuedEvents.h"
#include "entt/fwd.hpp"

namespace AM
{
struct ObjectSpriteSet;

namespace Client 
{
class WorldObjectLocator;
class Network;

/**
 * TODO: Add Change Position and Change Rotation buttons
 *       Make highlights look better
 */

/**
 * The build mode tool used for adding object entities (dynamic objects).
 */
class DynamicObjectTool : public BuildTool
{
public:
    DynamicObjectTool(World& inWorld,
               const WorldObjectLocator& inWorldObjectLocator,
               Network& inNetwork);

    /**
     * Sets the currently selected dynamic object template. This selection will 
     * follow the user's mouse, and will be placed if the user left clicks.
     *
     * This is called by DynamicObjectPanelContent when a template is selected.
     */
    void setSelectedTemplate(const std::string& name, const Rotation& rotation,
                             const ObjectSpriteSet& spriteSet);

    /**
     * @param inOnObjectSelected  A callback for when the user clicks on a 
     *                            dynamic object that isn't already selected.
     */
    void setOnObjectSelected(
        std::function<void(entt::entity objectEntityID)> inOnObjectSelected);

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
    // Note: This is called when the cursor leaves the BuildOverlay.
    void onMouseLeave() override;

private:
    /**
     * If the given entity is a dynamic object and isn't already selected, 
     * selects it and calls onObjectSelected.
     */
    void trySelectObject(entt::entity entity);

    /**
     * Listens for if the current selected object is destroyed.
     */
    void onEntityDestroyed(entt::registry& registry, entt::entity entity);

    /**
     * If we have an entity or template selected, clears it and calls 
     * onSelectionCleared.
     */
    void clearCurrentSelection();

    /**
     * Returns the selected template's sprite, or nullptr if there's no selected 
     * template.
     */
    const Sprite* getSelectedTemplateSprite();

    /** Used for finding objects that the mouse is hovering over or 
        clicking. */
    const WorldObjectLocator& worldObjectLocator;

    /** The color used to highlight the hovered object. */
    const SDL_Color highlightColor;

    // Note: We only have either an object or a template selected at one time.
    /** The selected object's ID. */
    entt::entity selectedObjectID;

    /** The selected template's name. */
    std::string selectedTemplateName;

    /** The selected template's sprite set. */
    const ObjectSpriteSet* selectedTemplateSpriteSet;

    /** The indices within selectedSpriteSet->sprites that contain a sprite. */
    std::vector<Uint8> selectedTemplateValidSpriteIndices;

    /** The selected template's sprite index. */
    std::size_t selectedTemplateSpriteIndex;

    std::function<void(entt::entity objectEntityID)> onObjectSelected;

    std::function<void(void)> onSelectionCleared;
};

} // End namespace Client
} // End namespace AM
