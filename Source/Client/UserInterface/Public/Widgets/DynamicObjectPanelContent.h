#pragma once

#include "MainButton.h"
#include "DynamicObjectTemplates.h"
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
class DynamicObjectTool;

/**
 * Content for the BuildPanel when the dynamic object tool is selected.
 */
class DynamicObjectPanelContent : public AUI::Widget
{
public:
    DynamicObjectPanelContent(World& inWorld,
                              Network& inNetwork,
                              SpriteData& inSpriteData,
                              BuildPanel& inBuildPanel,
                              const SDL_Rect& inScreenExtent,
                              const std::string& inDebugName
                              = "DynamicObjectPanelContent");

    /**
     * Sets the given tool as the current build tool.
     * Nullptr may be given, to say that we left dynamic object mode.
     */
    void setBuildTool(DynamicObjectTool* inDynamicObjectTool);

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

private:
    /**
     * Sets the current object for the edit view.
     */
    void setObjectToEdit(entt::entity newEditingObjectID);

    enum class ViewType {
        /** The default view, shows the available object templates. */
        Template,
        /** The edit view, for when an object is selected. */
        Edit,
        /** The sprite set view, for changing an object's sprite set. */
        SpriteSet
    };

    /**
     * Changes the current view to the given view.
     */
    void changeView(ViewType newView);

    /**
     * Add the "add dynamic object" thumbnail to the templates container.
     */
    void addAddThumbnail();

    /**
     * Fills the templates container with the given object templates.
     */
    void addTemplateThumbnails(
        const DynamicObjectTemplates& objectTemplates);

    /**
     * Fills the sprite set container with all of the object sprite sets.
     */
    void addSpriteSetThumbnails();

    /** Used for getting the current editing object's component data. */
    World& world;

    /** Used for sending dynamic object init requests to the server. */
    Network& network;

    /** Used to get the sprite sets that we fill the panel with. */
    SpriteData& spriteData;

    /** Used to properly deselect thumbnails when a new one is selected. */
    BuildPanel& buildPanel;

    /** The current content view type. */
    ViewType currentView;

    /** Used to tell the tool when a thumbnail is selected, and to register for 
        "a dynamic object was clicked" callbacks. */
    DynamicObjectTool* dynamicObjectTool;

    /** The ID of the object that we're currently editing. */
    entt::entity editingObjectID;

    /** The init script of the object that we're currently editing. */
    std::string editingObjectInitScript;

    /** The currently selected sprite thumbnail in the "change sprite" view. */
    AUI::Thumbnail* selectedSpriteThumbnail;

    /** Maps a sprite to the thumbnail that represents it. */
    std::unordered_map<const Sprite*, AUI::Thumbnail*> spriteThumbnailMap;

    EventQueue<DynamicObjectTemplates> objectTemplatesQueue;

    EventQueue<InitScriptResponse> initScriptQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    // Template view
    /** Holds the templates and the "New Dynamic Object" button. */
    AUI::VerticalGridContainer templateContainer;

    // Edit view
    AUI::Text nameLabel;

    AUI::TextInput nameInput;

    MainButton changeSpriteButton;

    MainButton changeScriptButton;

    MainButton saveTemplateButton;

    // SpriteSet selection view
    /** Holds the dynamic object sprite sets that are used to change a 
        selected object's sprite. */
    AUI::VerticalGridContainer spriteSetContainer;
};

} // End namespace Client
} // End namespace AM
