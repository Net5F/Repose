#pragma once

#include "MainButton.h"
#include "QueuedEvents.h"
#include "DynamicObjectTemplates.h"
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
class DynamicObjectTool;

/**
 * Content for the BuildPanel when the dynamic object tool is selected.
 */
class DynamicObjectPanelContent : public AUI::Widget
{
public:
    DynamicObjectPanelContent(EventDispatcher& inNetworkEventDispatcher,
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
    void setObjectToEdit(entt::entity ID, std::string name,
                         const ObjectSpriteSet& spriteSet, Uint8 spriteIndex);

    /**
     * Opens the "dynamic object edit view" and hides the template container.
     */
    void openEditView();

    /**
     * Closes the edit view, showing the template container again.
     */
    void closeEditView();

    /**
     * Add the "add dynamic object" thumbnail to the templates container.
     */
    void addAddThumbnail();

    /**
     * Fills the templates container with the given object templates.
     */
    void addTemplateThumbnails(
        const DynamicObjectTemplates& objectTemplates);

    // TODO: Add function for adding sprites to spriteContainer. Make sure 
    //       to blow out each sprite set into the individual sprites.

    /** Used to get the sprite sets that we fill the panel with. */
    SpriteData& spriteData;

    /** Used to properly deselect thumbnails when a new one is selected. */
    BuildPanel& buildPanel;

    /** Used to tell the tool when a thumbnail is selected, and to register for 
        "a dynamic object was clicked" callbacks. */
    DynamicObjectTool* dynamicObjectTool;

    /** The ID of the object that we're currently editing. */
    entt::entity editingObjectID;

    /** The name of the object that we're currently editing. */
    std::string editingObjectName;

    /** The sprite set of the object that we're currently editing. */
    const ObjectSpriteSet* editingObjectSpriteSet;

    /** The sprite index of the object that we're currently editing. */
    Uint8 editingObjectSpriteIndex;

    /** The currently selected sprite thumbnail in the "change sprite" view. */
    AUI::Thumbnail* selectedSpriteThumbnail;

    /** Maps a sprite to the thumbnail that represents it. */
    std::unordered_map<const Sprite*, AUI::Thumbnail*> spriteThumbnailMap;

    EventQueue<DynamicObjectTemplates> objectTemplatesQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    // Normal view
    /** Holds the templates and the "New Dynamic Object" button. */
    AUI::VerticalGridContainer templateContainer;

    /** Holds the dynamic object sprite sets. */
    AUI::VerticalGridContainer spriteSetContainer;

    // Edit view
    AUI::Text nameLabel;

    AUI::TextInput nameInput;

    MainButton changeScriptButton;

    MainButton saveTemplateButton;
};

} // End namespace Client
} // End namespace AM
