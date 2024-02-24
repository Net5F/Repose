#pragma once

#include "BuildTool.h"
#include "QueuedEvents.h"
#include "Name.h"
#include "GraphicState.h"
#include "entt/fwd.hpp"

namespace AM
{
struct ObjectGraphicSet;

namespace Client
{
class WorldObjectLocator;
class Network;
class GraphicData;

/**
 * TODO: Add Change Position button
 *       Make highlights look better
 */

// TODO: This uses object sprite sets everywhere. When we add character sprite
//       sets, figure out how the UI should surface them.
/**
 * The build mode tool used for adding entities.
 */
class EntityTool : public BuildTool
{
public:
    EntityTool(World& inWorld, const WorldObjectLocator& inWorldObjectLocator,
               Network& inNetwork, GraphicData& inGraphicData);

    /**
     * Sets the currently selected entity template. This selection will follow
     * the user's mouse, and will be placed if the user left clicks.
     *
     * This is called by EntityPanelContent when a template is selected.
     */
    void setSelectedTemplate(const Name& name,
                             const GraphicState& graphicState);

    /**
     * @param inOnEntitySelected  A callback for when the user clicks on a
     *                            entity that isn't already selected.
     */
    void setOnEntitySelected(
        std::function<void(entt::entity entity)> inOnEntitySelected);

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
     * If the given entity isn't a player entity and isn't already selected,
     * selects it and calls onEntitySelected.
     */
    void trySelectEntity(entt::entity entity);

    /**
     * Listens for if the current selected entity is destroyed.
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

    /** Used for finding entities that the mouse is hovering over or
        clicking. */
    const WorldObjectLocator& worldObjectLocator;
    /** Used to get graphics from AnimationStates. */
    GraphicData& graphicData;

    /** The color used to highlight the hovered entity. */
    const SDL_Color highlightColor;

    // Note: We only have either an entity or a template selected at one time.
    /** The selected entity's ID. */
    entt::entity selectedEntityID;

    /** The selected template's name. */
    Name selectedTemplateName;

    /** The selected template's graphic set and graphic index. */
    GraphicState selectedTemplateGraphicState;

    std::function<void(entt::entity entity)> onEntitySelected;

    std::function<void(void)> onSelectionCleared;
};

} // End namespace Client
} // End namespace AM
