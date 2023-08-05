#pragma once

#include "MainButton.h"
#include "QueuedEvents.h"
#include "EntityTemplates.h"
#include "AUI/Widget.h"
#include "AUI/Text.h"
#include "AUI/TextInput.h"
#include "AUI/VerticalGridContainer.h"
#include "entt/fwd.hpp"

namespace AUI
{
class Thumbnail;
}

namespace AM
{
struct SpriteSet;
struct ObjectSpriteSet;
struct Sprite;

namespace Client
{
class SpriteData;
class BuildPanel;
class EntityTool;

/**
 * Content for the BuildPanel when the Entity tool is selected.
 */
class EntityPanelContent : public AUI::Widget
{
public:
    EntityPanelContent(EventDispatcher& inNetworkEventDispatcher,
                              SpriteData& inSpriteData,
                              BuildPanel& inBuildPanel,
                              const SDL_Rect& inScreenExtent,
                              const std::string& inDebugName
                              = "EntityPanelContent");

    /**
     * Sets the given tool as the current build tool.
     * Nullptr may be given, to say that we left entity mode.
     */
    void setBuildTool(EntityTool* inEntityTool);

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

private:
    /**
     * Sets the current object for the edit view.
     */
    void setObjectToEdit(entt::entity ID, std::string name,
                         const ObjectSpriteSet& spriteSet, Uint8 spriteIndex);

    /**
     * Opens the "entity edit view" and hides the template container.
     */
    void openEditView();

    /**
     * Closes the edit view, showing the template container again.
     */
    void closeEditView();

    /**
     * Add the "add entity" thumbnail to the templates container.
     */
    void addAddThumbnail();

    /**
     * Fills the templates container with the given object templates.
     */
    void addTemplateThumbnails(
        const EntityTemplates& entityTemplates);

    // TODO: Add function for adding sprites to spriteContainer. Make sure 
    //       to blow out each sprite set into the individual sprites.

    /** Used to get the sprite sets that we fill the panel with. */
    SpriteData& spriteData;

    /** Used to properly deselect thumbnails when a new one is selected. */
    BuildPanel& buildPanel;

    /** Used to tell the tool when a thumbnail is selected, and to register for 
        "an entity was clicked" callbacks. */
    EntityTool* entityTool;

    /** The ID of the entity that we're currently editing. */
    entt::entity editingEntityID;

    /** The name of the entity that we're currently editing. */
    std::string editingEntityName;

    /** The sprite set of the entity that we're currently editing. */
    const ObjectSpriteSet* editingEntitySpriteSet;

    /** The sprite index of the entity that we're currently editing. */
    Uint8 editingEntitySpriteIndex;

    /** The currently selected sprite thumbnail in the "change sprite" view. */
    AUI::Thumbnail* selectedSpriteThumbnail;

    /** Maps a sprite to the thumbnail that represents it. */
    std::unordered_map<const Sprite*, AUI::Thumbnail*> spriteThumbnailMap;

    EventQueue<EntityTemplates> entityTemplatesQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    // Normal view
    /** Holds the templates and the "New Object Entity" button. */
    AUI::VerticalGridContainer templateContainer;

    // Edit view
    AUI::Text nameLabel;

    AUI::TextInput nameInput;

    MainButton changeScriptButton;

    MainButton saveTemplateButton;
};

} // End namespace Client
} // End namespace AM
