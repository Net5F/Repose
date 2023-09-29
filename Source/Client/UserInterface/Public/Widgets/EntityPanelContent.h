#pragma once

#include "MainButton.h"
#include "EntityTemplates.h"
#include "InitScriptResponse.h"
#include "QueuedEvents.h"
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
class World;
class Network;
class SpriteData;
class BuildPanel;
class EntityTool;

// TODO: This uses object sprite sets everywhere. When we add character sprite 
//       sets, figure out how the UI should surface them.
/**
 * Content for the BuildPanel when the entity tool is selected.
 */
class EntityPanelContent : public AUI::Widget
{
public:
    EntityPanelContent(World& inWorld, Network& inNetwork,
                       SpriteData& inSpriteData, BuildPanel& inBuildPanel,
                       const SDL_Rect& inScreenExtent,
                       const std::string& inDebugName = "EntityPanelContent");

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
     * Sets the current entity for the edit view.
     */
    void setEntityToEdit(entt::entity newEditingEntityID);

    enum class ViewType {
        /** The default view, shows the available entity templates. */
        Template,
        /** The edit view, for when an entity is selected. */
        Edit,
        /** The sprite set view, for changing an entity's sprite set. */
        SpriteSet
    };

    /**
     * Changes the current view to the given view.
     */
    void changeView(ViewType newView);

    /**
     * Add the "add entity" thumbnail to the templates container.
     */
    void addAddThumbnail();

    /**
     * Fills the templates container with the given entity templates.
     */
    void addTemplateThumbnails(
        const EntityTemplates& entityTemplates);

    /**
     * Fills the sprite set container with all of the entity sprite sets.
     */
    void addSpriteSetThumbnails();

    /** Used for getting the current editing entity's component data. */
    World& world;

    /** Used for sending entity init requests to the server. */
    Network& network;

    /** Used to get the sprite sets that we fill the panel with. */
    SpriteData& spriteData;

    /** Used to properly deselect thumbnails when a new one is selected. */
    BuildPanel& buildPanel;

    /** The current content view type. */
    ViewType currentView;

    /** Used to tell the tool when a thumbnail is selected, and to register for 
        "a entity was clicked" callbacks. */
    EntityTool* entityTool;

    /** The ID of the entity that we're currently editing. */
    entt::entity editingEntityID;

    /** The init script of the entity that we're currently editing. */
    std::string editingEntityInitScript;

    /** The currently selected sprite thumbnail in the "change sprite" view. */
    AUI::Thumbnail* selectedSpriteThumbnail;

    /** Maps a sprite to the thumbnail that represents it. */
    std::unordered_map<const Sprite*, AUI::Thumbnail*> spriteThumbnailMap;

    EventQueue<EntityTemplates> entityTemplatesQueue;

    EventQueue<InitScriptResponse> initScriptQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    // Template view
    /** Holds the templates and the "New Entity" button. */
    AUI::VerticalGridContainer templateContainer;

    // Edit view
    AUI::Text nameLabel;

    AUI::TextInput nameInput;

    MainButton changeSpriteButton;

    MainButton changeScriptButton;

    MainButton saveTemplateButton;

    // SpriteSet selection view
    /** Holds the sprite sets that are used to change a selected entity's 
        sprite. */
    AUI::VerticalGridContainer spriteSetContainer;
};

} // End namespace Client
} // End namespace AM