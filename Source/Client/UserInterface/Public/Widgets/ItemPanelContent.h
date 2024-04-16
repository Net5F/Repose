#pragma once

#include "MainButton.h"
#include "ItemID.h"
#include "ItemError.h"
#include "ItemInitScriptResponse.h"
#include "AUI/Widget.h"
#include "AUI/Text.h"
#include "AUI/TextInput.h"
#include "AUI/Image.h"
#include "AUI/VerticalGridContainer.h"
#include "QueuedEvents.h"
#include <string_view>

namespace AUI
{
class Thumbnail;
}

namespace AM
{
struct Item;

namespace Client
{
class Simulation;
class World;
class Network;
class IconData;

/**
 * Content for the BuildPanel when the item tool is selected.
 */
class ItemPanelContent : public AUI::Widget
{
public:
    ItemPanelContent(Simulation& inSimulation, Network& inNetwork,
                     IconData& inIconData, const SDL_Rect& inScreenExtent,
                     const std::string& inDebugName = "ItemPanelContent");

    /**
     * Resets this widget to its default state.
     * Used by BuildPanel when the "Item" button is clicked.
     */
    void reset();

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

private:
    /**
     * Sets the given item as our requested item and sends a request to the
     * server for the item's latest definition.
     */
    void trySelectItem(std::string_view displayName);

    /**
     * If we have a pending requested item and this matches it, finishes
     * selecting the item and fills out the UI.
     *
     * Also refreshes itemCacheContainer.
     */
    void onItemUpdate(const Item& item);

    /**
     * If we have a selected item and have received its init script, sends
     * an init request to the server.
     */
    void sendItemChangeRequest();

    enum class ViewType {
        /** The default view, used to search for items. */
        Home,
        /** Shows the local item cache as a list of thumbnails. */
        ItemCache,
        /** Used to create items. */
        Create,
        /** Used to edit items. */
        Edit,
        /* Shows the list of available icons. */
        IconList,
        /** Used to duplicate items. */
        Duplicate
    };

    /**
     * Changes the current view to the given view.
     */
    void changeView(ViewType newView);

    /**
     * Fills the item container with all of our cached items.
     */
    void refreshItemCacheThumbnails();

    /**
     * Fills the icon container will all of the available icons.
     */
    void addIconThumbnails();

    // These all make their widgets visible and set any needed callbacks.
    void showHomeView();
    void showCreateView();
    void showEditView();
    void showDuplicateView();

    /** Used for getting the current editing entity's component data. */
    World& world;

    /** Used for sending entity init requests to the server. */
    Network& network;

    /** Used to get the item icons. */
    IconData& iconData;

    /** The current content view type. */
    ViewType currentView;

    /** The string ID of the item definition that we've requested.
        When an item is staged to be selected and we're waiting for a response
        from the server, this will be filled. */
    std::string requestedItemStringID;

    /** The item that we currently have selected.
        If == NULL_ITEM_ID, we don't have an item selected. */
    ItemID selectedItemID;

    /** The display name of the item that we currently have selected. */
    std::string selectedItemDisplayName;

    /** The icon of the item that we currently have selected. */
    IconID selectedItemIconID;

    /** The init script of the item that we currently have selected. */
    std::string selectedItemInitScript;

    /** If true, we're editing an item and have received its init script (and
        saved it in editinItemInitScript). */
    bool initScriptReceived;

    EventQueue<ItemError> itemErrorQueue;
    EventQueue<ItemInitScriptResponse> itemInitScriptQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    // Shared
    /** Used for displaying the name of the current view. */
    AUI::Text nameLabel;

    /** Used for entering an item name. */
    AUI::TextInput nameInput;

    /** Used for displaying the currently selected item's icon. */
    AUI::Image itemIconImage;

    /** Used for the "Create" and "Duplicate" buttons. */
    MainButton centerButton;

    /** Used for showing error text, such as "Name already in use." */
    AUI::Text errorLabel;

    // Home view
    MainButton viewCacheButton;
    MainButton createNewButton;
    /** Used for "Edit" in Home view, and "Open Script" in Edit view. */
    MainButton rightButton1;
    /** Used for "Duplicate" in Home view, and "Commit Script" in Edit view. */
    MainButton rightButton2;
    /** Used for "Give" in Home view, and "Select Icon" in Edit view. */
    MainButton rightButton3;

    AUI::Text itemNotFoundLabel;

    // Cache view
    /** Holds thumbnails for our cached items. */
    AUI::VerticalGridContainer itemCacheContainer;

    // IconList view
    AUI::VerticalGridContainer iconContainer;
};

} // End namespace Client
} // End namespace AM
