#include "ItemPanelContent.h"
#include "Simulation.h"
#include "World.h"
#include "Network.h"
#include "Item.h"
#include "IconData.h"
#include "BuildPanel.h"
#include "BuildModeThumbnail.h"
#include "ItemDataRequest.h"
#include "ItemError.h"
#include "ItemInitScriptRequest.h"
#include "ItemInitRequest.h"
#include "ItemChangeRequest.h"
#include "InventoryOperation.h"
#include "Paths.h"
#include <SDL.h>
#include <fstream>
#include <sstream>

namespace AM
{
namespace Client
{
ItemPanelContent::ItemPanelContent(Simulation& inSimulation, Network& inNetwork,
                                   IconData& inIconData,
                                   const SDL_Rect& inScreenExtent,
                                   const std::string& inDebugName)
: AUI::Widget(inScreenExtent, inDebugName)
, world{inSimulation.getWorld()}
, network{inNetwork}
, iconData{inIconData}
, currentView{ViewType::Home}
, requestedItemStringID{}
, selectedItemID{NULL_ITEM_ID}
, selectedItemDisplayName{}
, selectedItemIconID{NULL_ICON_ID}
, selectedItemInitScript{}
, initScriptReceived{false}
, itemErrorQueue{inNetwork.getEventDispatcher()}
, itemInitScriptQueue{inNetwork.getEventDispatcher()}
// Note: These dimensions are based on the top left that BuildPanel gives us.
, nameLabel{{464, 8, 260, 36}, "NameLabel"}
, nameInput{{464, 53, 260, 42}, "NameInput"}
, itemIconImage{{554, 108, 80, 80}, "ItemIconImage"}
, centerButton{{524, 117, 140, 36}, "Init", "CenterButton"}
, errorLabel{{232, 165, 722, 36}, "ErrorLabel"}
, viewCacheButton{{311, 53, 140, 36}, "View Cache", "ViewCacheButton"}
, createNewButton{{311, 99, 140, 36}, "Create New", "CreateNewButton"}
, rightButton1{{738, 53, 140, 36}, "", "RightButton1"}
, rightButton2{{738, 99, 140, 36}, "", "RightButton2"}
, rightButton3{{738, 145, 140, 36}, "", "RightButton3"}
, itemNotFoundLabel{{464, 113, 260, 36}, "ItemNotFoundLabel"}
, itemCacheContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                     "ItemCacheContainer"}
, iconContainer{{0, 0, logicalExtent.w, logicalExtent.h}, "IconContainer"}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(nameLabel);
    children.push_back(nameInput);
    children.push_back(itemIconImage);
    children.push_back(centerButton);
    children.push_back(errorLabel);
    children.push_back(viewCacheButton);
    children.push_back(createNewButton);
    children.push_back(rightButton1);
    children.push_back(rightButton2);
    children.push_back(rightButton3);
    children.push_back(itemNotFoundLabel);
    children.push_back(itemCacheContainer);
    children.push_back(iconContainer);

    /* Label */
    auto setTextStyle = [](AUI::Text& text) {
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
        text.setColor({255, 255, 255, 255});
        text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
        text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    };
    setTextStyle(nameLabel);
    setTextStyle(errorLabel);
    setTextStyle(itemNotFoundLabel);
    itemNotFoundLabel.setText("Item not found.");

    /* Text input */
    nameInput.normalImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
    nameInput.hoveredImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Hovered.png"), {8, 8, 8, 8});
    nameInput.focusedImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Focused.png"), {8, 8, 8, 8});
    nameInput.setTextFont(((Paths::FONT_DIR + "Cagliostro-Regular.ttf")), 20);
    nameInput.setTextColor({255, 255, 255, 255});
    nameInput.setPadding({0, 14, 0, 14});
    nameInput.setCursorWidth(2);
    nameInput.setCursorColor({255, 255, 255, 255});

    /* Buttons */
    viewCacheButton.setOnPressed([this]() { changeView(ViewType::ItemCache); });

    createNewButton.setOnPressed([this]() { changeView(ViewType::Create); });

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
    };
    setContainerStyle(itemCacheContainer);
    setContainerStyle(iconContainer);

    // Add the item and icon thumbnails.
    refreshItemCacheThumbnails();
    addIconThumbnails();

    // Set up the home view.
    changeView(ViewType::Home);

    // If an item that we're displaying (or trying to display) is updated, we
    // need to update this widget.
    inSimulation.getItemUpdateSink().connect<&ItemPanelContent::onItemUpdate>(
        *this);
}

void ItemPanelContent::reset()
{
    changeView(ViewType::Home);
}

void ItemPanelContent::onTick(double)
{
    // Process any waiting messages, displaying any errors appropriately.
    ItemError itemError{};
    while (itemErrorQueue.pop(itemError)) {
        if ((currentView == ViewType::Home)
            && (itemError.errorType == ItemError::StringIDNotFound)
            && (itemError.stringID == requestedItemStringID)) {
            requestedItemStringID = "";
            itemIconImage.setIsVisible(false);
            rightButton1.setIsVisible(false);
            rightButton2.setIsVisible(false);
            rightButton3.setIsVisible(false);
            itemNotFoundLabel.setIsVisible(true);
        }
        else if (((currentView == ViewType::Create)
                  || (currentView == ViewType::Edit)
                  || (currentView == ViewType::Duplicate))
                 && (itemError.errorType == ItemError::StringIDInUse)) {
            errorLabel.setText("Error: Name already in use.");
            errorLabel.setIsVisible(true);
        }
        else if (currentView == ViewType::Edit) {
            if (itemError.errorType == ItemError::PermissionFailure) {
                errorLabel.setText(
                    "Error: You don't have permission to edit this item.");
            }
            else if (itemError.errorType == ItemError::InitScriptFailure) {
                errorLabel.setText(
                    "Error: Invalid script (see chat for details).");
            }
        }
        else if (itemError.errorType == ItemError::NumericIDNotFound) {
            AM_ASSERT(false, "Numeric ID wasn't found while editing item.");
        }
    }

    ItemInitScriptResponse initScriptResponse{};
    while (itemInitScriptQueue.pop(initScriptResponse)) {
        // If the received script is for the currently selected item, save it.
        if (initScriptResponse.itemID == selectedItemID) {
            selectedItemInitScript = initScriptResponse.initScript.script;
            initScriptReceived = true;

            // Write to InitScript.lua
            std::ofstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
            scriptFile << initScriptResponse.initScript.script;
            LOG_INFO("Received item script. Saved to InitScript.lua");
        }
    }
}

void ItemPanelContent::trySelectItem(std::string_view displayName)
{
    // If the given name is empty, do nothing.
    if (displayName == "") {
        return;
    }

    // Deselect any current selection.
    selectedItemID = NULL_ITEM_ID;
    selectedItemDisplayName = "";
    selectedItemIconID = NULL_ICON_ID;
    selectedItemInitScript = "";

    // Track that we're requesting this item.
    std::string stringID{ItemData::deriveStringID(displayName)};
    requestedItemStringID = stringID;

    // Ask the server for the latest definition for this item.
    network.serializeAndSend(ItemDataRequest{stringID});
}

void ItemPanelContent::onItemUpdate(const Item& item)
{
    // Note: We handle this signal instead of just receiving the update message
    //       in the UI, so that we can be sure ItemData is up-to-date before we
    //       call getAllItems().

    // If we requested this item, select it.
    if (requestedItemStringID == item.stringID) {
        // Select the item.
        selectedItemID = item.numericID;
        selectedItemDisplayName = item.displayName;
        selectedItemIconID = item.iconID;

        // Update the UI.
        nameInput.setText(item.displayName);
        const IconRenderData& iconRenderData{
            iconData.getRenderData(item.iconID)};
        itemIconImage.setSimpleImage(iconRenderData.iconSheetRelPath,
                                     iconRenderData.textureExtent);
        itemNotFoundLabel.setIsVisible(false);
        itemIconImage.setIsVisible(true);
        rightButton1.setIsVisible(true);
        rightButton2.setIsVisible(true);
        rightButton3.setIsVisible(true);

        // Make sure we're on the home view to show the newly selected item.
        changeView(ViewType::Home);

        // Request the item's init script.
        network.serializeAndSend(ItemInitScriptRequest{selectedItemID});

        requestedItemStringID = "";
    }
    // If this is the currently selected item, update it.
    else if (item.numericID == selectedItemID) {
        // Update our state and the UI.
        selectedItemDisplayName = item.displayName;
        selectedItemIconID = item.iconID;

        nameInput.setText(item.displayName);
        const IconRenderData& iconRenderData{
            iconData.getRenderData(item.iconID)};
        itemIconImage.setSimpleImage(iconRenderData.iconSheetRelPath,
                                     iconRenderData.textureExtent);
        errorLabel.setIsVisible(false);

        // Request the item's new init script.
        initScriptReceived = false;
        network.serializeAndSend(ItemInitScriptRequest{selectedItemID});
    }

    // Update the cache.
    refreshItemCacheThumbnails();
}

void ItemPanelContent::sendItemChangeRequest()
{
    // Don't send a change request if we haven't yet received the init script,
    // since we would end up overwriting the current one.
    if (initScriptReceived) {
        // Send a re-init request with the updated name.
        std::ifstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
        if (scriptFile.is_open()) {
            std::stringstream buffer;
            buffer << scriptFile.rdbuf();

            const std::string& initScript{buffer.str()};
            network.serializeAndSend(
                ItemChangeRequest{selectedItemID, nameInput.getText(),
                                  selectedItemIconID, initScript});
        }
        else {
            LOG_INFO("Failed to open InitScript.lua");
        }
    }
}

void ItemPanelContent::changeView(ViewType newView)
{
    // Hide everything.
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    itemIconImage.setIsVisible(false);
    centerButton.setIsVisible(false);
    errorLabel.setIsVisible(false);
    viewCacheButton.setIsVisible(false);
    createNewButton.setIsVisible(false);
    rightButton1.setIsVisible(false);
    rightButton2.setIsVisible(false);
    rightButton3.setIsVisible(false);
    itemNotFoundLabel.setIsVisible(false);
    itemCacheContainer.setIsVisible(false);
    iconContainer.setIsVisible(false);

    // Show the new view's widgets.
    switch (newView) {
        case ViewType::ItemCache:
            itemCacheContainer.setIsVisible(true);
            break;
        case ViewType::Create:
            showCreateView();
            break;
        case ViewType::Edit:
            showEditView();
            break;
        case ViewType::IconList:
            iconContainer.setIsVisible(true);
            break;
        case ViewType::Duplicate:
            showDuplicateView();
            break;
        case ViewType::Home:
        default:
            showHomeView();
            break;
    }

    currentView = newView;
}

void ItemPanelContent::refreshItemCacheThumbnails()
{
    itemCacheContainer.clear();

    // Add thumbnails for all cached items.
    for (const auto& [key, item] : world.itemData.getAllItems()) {
        // Skip the null item.
        if (!(item.numericID)) {
            continue;
        }

        // Construct the new thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<BuildModeThumbnail>("ItemThumbnail")};
        BuildModeThumbnail& thumbnail{
            static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Load the item's icon.
        const IconRenderData& iconRenderData{
            iconData.getRenderData(item.iconID)};
        thumbnail.thumbnailImage.setSimpleImage(iconRenderData.iconSheetRelPath,
                                                iconRenderData.textureExtent);

        // When this thumbnail is selected, select the associated item and
        // switch back to the home view.
        thumbnail.setOnSelected(
            [&, itemName{item.displayName}](AUI::Thumbnail*) {
                trySelectItem(itemName);

                changeView(ViewType::Home);
            });

        itemCacheContainer.push_back(std::move(thumbnailPtr));
    }
}

void ItemPanelContent::addIconThumbnails()
{
    // Add thumbnails for all icons.
    for (const Icon& icon : iconData.getAllIcons()) {
        // Construct the new thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<BuildModeThumbnail>("IconThumbnail")};
        BuildModeThumbnail& thumbnail{
            static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Load the icon.
        const IconRenderData& iconRenderData{
            iconData.getRenderData(icon.numericID)};
        thumbnail.thumbnailImage.setSimpleImage(iconRenderData.iconSheetRelPath,
                                                iconRenderData.textureExtent);

        // When this thumbnail is selected, select the associated icon and
        // switch back to the edit view.
        thumbnail.setOnMouseDown(
            [&, iconID{icon.numericID}](AUI::Thumbnail*,
                                        AUI::MouseButtonType buttonType) {
                if (buttonType == AUI::MouseButtonType::Left) {
                    // Set the new icon and request the server update the item.
                    selectedItemIconID = iconID;
                    sendItemChangeRequest();

                    changeView(ViewType::Edit);

                    return true;
                }
                else {
                    return false;
                }
            });

        iconContainer.push_back(std::move(thumbnailPtr));
    }
}

void ItemPanelContent::showHomeView()
{
    // Set our widget behaviors.
    nameLabel.setText("Item Name");

    nameInput.setOnTextCommitted([&]() { trySelectItem(nameInput.getText()); });

    rightButton1.text.setText("Edit");
    rightButton1.setOnPressed([this]() { changeView(ViewType::Edit); });

    rightButton2.text.setText("Duplicate");
    rightButton2.setOnPressed([this]() { changeView(ViewType::Duplicate); });

    rightButton3.text.setText("Give");
    rightButton3.setOnPressed([this]() {
        AM_ASSERT(selectedItemID != NULL_ITEM_ID,
                  "Somehow selected Give without a selected item.");

        // Ask the server to add the selected item to the player's inventory.
        network.serializeAndSend(InventoryOperation{
            InventoryAddItem{world.playerEntity, selectedItemID, 1}});
    });

    // Put the error label in the correct place for this view.
    SDL_Rect errorLabelExtent{errorLabel.getLogicalExtent()};
    errorLabelExtent.y = 165;
    errorLabel.setLogicalExtent(errorLabelExtent);

    // If we have an item selected, show it.
    if (selectedItemID != NULL_ITEM_ID) {
        nameLabel.setText(selectedItemDisplayName);
        nameInput.setText(selectedItemDisplayName);
        itemIconImage.setIsVisible(true);
        rightButton1.setIsVisible(true);
        rightButton2.setIsVisible(true);
        rightButton3.setIsVisible(true);
    }
    else {
        // No selection, reset our state.
        nameInput.setText("");
        itemIconImage.setIsVisible(false);
        itemNotFoundLabel.setIsVisible(false);
    }

    // Make the proper widgets visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    viewCacheButton.setIsVisible(true);
    createNewButton.setIsVisible(true);
}

void ItemPanelContent::showCreateView()
{
    // Set our widget behaviors.
    nameLabel.setText("Create Item");
    nameInput.setText("");

    // Do nothing when text is committed (we'll read the committed name when
    // the "Create" button is pressed).
    nameInput.setOnTextCommitted([]() {});

    centerButton.text.setText("Create");
    centerButton.setOnPressed([this]() {
        // Ask the server to create a blank item with the given name.
        network.serializeAndSend(
            ItemInitRequest{nameInput.getText(), NULL_ICON_ID, ""});

        // Track that we're requesting this item.
        std::string stringID{ItemData::deriveStringID(nameInput.getText())};
        requestedItemStringID = stringID;
    });

    // Put the error label in the correct place for this view.
    SDL_Rect errorLabelExtent{errorLabel.getLogicalExtent()};
    errorLabelExtent.y = 165;
    errorLabel.setLogicalExtent(errorLabelExtent);

    // Make the proper widgets visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    centerButton.setIsVisible(true);
    nameInput.setIsVisible(true);
    centerButton.setIsVisible(true);
}

void ItemPanelContent::showEditView()
{
    // Set our widget behaviors.
    nameLabel.setText("Edit Item");

    nameInput.setOnTextCommitted([this]() {
        // Only update if the name actually changed.
        if (nameInput.getText() != selectedItemDisplayName) {
            sendItemChangeRequest();
        }
    });

    rightButton1.text.setText("Open Script");
    rightButton1.setOnPressed([this]() {
        std::string path{Paths::BASE_PATH + "InitScript.lua"};
        SDL_OpenURL(path.c_str());
    });

    rightButton2.text.setText("Commit Script");
    rightButton2.setOnPressed([this]() { sendItemChangeRequest(); });

    rightButton3.text.setText("Select Icon");
    rightButton3.setOnPressed([this]() { changeView(ViewType::IconList); });

    // Put the error label in the correct place for this view.
    SDL_Rect errorLabelExtent{errorLabel.getLogicalExtent()};
    errorLabelExtent.y = 187;
    errorLabel.setLogicalExtent(errorLabelExtent);

    // Make the proper widgets visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    itemIconImage.setIsVisible(true);
    rightButton1.setIsVisible(true);
    rightButton2.setIsVisible(true);
    rightButton3.setIsVisible(true);
}

void ItemPanelContent::showDuplicateView()
{
    AM_ASSERT(selectedItemID != NULL_ITEM_ID,
              "Somehow reached Duplicate view without a selected item.");

    // Set our widget behaviors.
    nameLabel.setText("New Name");
    nameInput.setText("");

    // Do nothing when text is committed (we'll read the committed name when
    // the "Duplicate" button is pressed).
    nameInput.setOnTextCommitted([]() {});

    centerButton.text.setText("Duplicate");
    centerButton.setOnPressed([this]() {
        // Ask the server to create a duplicate item with the given name.
        network.serializeAndSend(ItemInitRequest{
            nameInput.getText(), selectedItemIconID, selectedItemInitScript});

        // Track that we're requesting this item.
        std::string stringID{ItemData::deriveStringID(nameInput.getText())};
        requestedItemStringID = stringID;
    });

    // Put the error label in the correct place for this view.
    SDL_Rect errorLabelExtent{errorLabel.getLogicalExtent()};
    errorLabelExtent.y = 165;
    errorLabel.setLogicalExtent(errorLabelExtent);

    // Make the proper widgets visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    centerButton.setIsVisible(true);
}

} // End namespace Client
} // End namespace AM
