#include "InventoryWindow.h"
#include "Simulation.h"
#include "Network.h"
#include "ItemData.h"
#include "IconData.h"
#include "InteractionManager.h"
#include "Paths.h"
#include "Inventory.h"
#include "ItemThumbnail.h"
#include "DragDropData.h"
#include "InventoryOperation.h"
#include "AUI/ScalingHelpers.h"
#include "entt/entity/registry.hpp"

namespace AM
{
namespace Client
{
InventoryWindow::InventoryWindow(Simulation& inSimulation, Network& inNetwork,
                                 ItemData& inItemData, IconData& inIconData,
                                 InteractionManager& inInteractionManager)
: AUI::Window({1362, 340, 256, 256}, "InventoryWindow")
, world{inSimulation.getWorld()}
, network{inNetwork}
, itemData{inItemData}
, iconData{inIconData}
, interactionManager{inInteractionManager}
, wasRefreshed{false}
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, slotContainer({12, 12, (logicalExtent.w - 24), (logicalExtent.h - 24)},
                "SlotContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(slotContainer);

    /* Background image */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});

    /* Item slot container */
    slotContainer.setNumColumns(4);
    slotContainer.setCellWidth(THUMBNAIL_SIZE);
    slotContainer.setCellHeight(THUMBNAIL_SIZE);

    // We need to update this widget when the player's inventory changes, or
    // when an item definition changes.
    world.registry.on_update<Inventory>()
        .connect<&InventoryWindow::onInventoryUpdated>(*this);
    itemData.itemCreated.connect<&InventoryWindow::onItemUpdate>(*this);
    itemData.itemUpdated.connect<&InventoryWindow::onItemUpdate>(*this);
}

void InventoryWindow::arrange()
{
    // Run the normal arrange step.
    Window::arrange();

    // Items in the inventory may have changed. If the mouse is hovering over
    // the inventory, update InteractionManager's hover state.
    // Note: We have to wait until after layout, so the thumbnails are in their
    //       actual position.
    if (wasRefreshed) {
        wasRefreshed = false;

        // Get the current mouse position and make it window-relative.
        SDL_Point cursorPosition{};
        SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
        cursorPosition.x -= scaledExtent.x;
        cursorPosition.y -= scaledExtent.y;

        // If the mouse isn't in the inventory, no items are being hovered and
        // none were being hovered before the update. Do nothing.
        if (!containsPoint(cursorPosition)) {
            return;
        }

        // If we're hovering an item thumbnail, tell InteractionManager.
        bool hoveringItem{false};
        for (Uint8 slotIndex = 0; slotIndex < slotContainer.size();
             ++slotIndex) {
            std::unique_ptr<AUI::Widget>& item{slotContainer[slotIndex]};
            if (item->getIsVisible() && item->containsPoint(cursorPosition)) {
                interactionManager.itemHovered(slotIndex);
                hoveringItem = true;
            }
        }

        // If we aren't hovering an item, tell InteractionManager to unhover
        // since we may been hovering an item before the update.
        if (!hoveringItem) {
            interactionManager.unhovered();
        }
    }
}

void InventoryWindow::refresh(const Inventory& inventory)
{
    // If an item was selected, deselect it.
    interactionManager.itemDeselected();

    // Add thumbnails for each slot in the player's inventory.
    slotContainer.clear();
    for (Uint8 slotIndex = 0; slotIndex < inventory.slots.size(); ++slotIndex) {
        // Assume that the slot is empty and add an empty thumbnail.
        ItemThumbnail& thumbnail{addEmptyThumbnail(slotIndex)};

        // If the slot has an item in it, build the empty thumbnail into a
        // full item thumbnail.
        const Inventory::ItemSlot& slot{inventory.slots[slotIndex]};
        if (slot.ID) {
            finishItemThumbnail(thumbnail, slot.ID, slot.count, slotIndex);
        }
    }

    wasRefreshed = true;
}

ItemThumbnail& InventoryWindow::addEmptyThumbnail(Uint8 slotIndex)
{
    // Construct the new thumbnail and add it to the item container.
    std::unique_ptr<AUI::Widget> thumbnailPtr{std::make_unique<ItemThumbnail>(
        SDL_Rect{0, 0, THUMBNAIL_SIZE, THUMBNAIL_SIZE}, "InventoryThumbnail")};
    slotContainer.push_back(std::move(thumbnailPtr));
    ItemThumbnail& thumbnail{
        static_cast<ItemThumbnail&>(*(slotContainer.back()))};

    // If an inventory item is dropped on this thumbnail, move the item into
    // this thumbnail's slot.
    thumbnail.setOnDrop([&, slotIndex](const DragDropData& dragDropData) {
        const auto* itemData{
            std::get_if<DragDropData::InventoryItem>(&(dragDropData.dataType))};
        if (itemData && (itemData->sourceSlotIndex != slotIndex)) {
            network.serializeAndSend(InventoryOperation{
                InventoryMoveItem{itemData->sourceSlotIndex, slotIndex}});
        }
    });

    return thumbnail;
}

void InventoryWindow::finishItemThumbnail(ItemThumbnail& thumbnail,
                                          ItemID itemID, Uint8 itemCount,
                                          Uint8 slotIndex)
{
    // Load the item's icon.
    const IconRenderData* renderData{nullptr};
    if (const Item* item{itemData.getItem(itemID)}) {
        renderData = &(iconData.getRenderData(item->iconID));
    }
    else {
        // Default icon
        renderData = &(iconData.getRenderData(NULL_ICON_ID));
    }
    thumbnail.thumbnailImage.setSimpleImage(renderData->iconSheetRelPath,
                                            renderData->textureExtent);
    thumbnail.dragDropImage.setSimpleImage(renderData->iconSheetRelPath,
                                           renderData->textureExtent);

    // If the slot is holding a stack of items, show the count text.
    if (itemCount > 1) {
        thumbnail.setItemCount(itemCount);
    }

    // Load the thumbnail's selected image.
    thumbnail.selectedImage.setNineSliceImage(
        Paths::TEXTURE_DIR + "Thumbnail/Selected_NineSlice.png", {4, 4, 4, 4});

    // Add the associated slot as the drag/drop payload.
    thumbnail.setDragDropData(
        std::make_unique<DragDropData>(DragDropData::InventoryItem{slotIndex}));

    // Add our callbacks.
    thumbnail.setOnHovered([&, slotIndex](ItemThumbnail*) {
        interactionManager.itemHovered(slotIndex);
    });
    thumbnail.setOnUnhovered([&, slotIndex](ItemThumbnail*) {
        interactionManager.unhovered();
    });
    thumbnail.setOnMouseDown([&, slotIndex](ItemThumbnail* thumbnail,
                                            AUI::MouseButtonType buttonType) {
        return interactionManager.itemMouseDown(slotIndex, buttonType,
                                                *thumbnail);
    });
    thumbnail.setOnMouseUp([&, slotIndex](ItemThumbnail* thumbnail,
                                          AUI::MouseButtonType buttonType) {
        interactionManager.itemMouseUp(slotIndex, buttonType, *thumbnail);
    });
    thumbnail.setOnDeselected([&, slotIndex](ItemThumbnail* thumbnail) {
        // Note: We keep our thumbnails non-focusable by default since
        //       we only want them focused when we Use them.
        //       InteractionManager handles setting isFocusable to true.
        thumbnail->setIsFocusable(false);
        interactionManager.itemDeselected();
    });
}

void InventoryWindow::onInventoryUpdated(entt::registry& registry,
                                         entt::entity entity)
{
    // We only care about updates to the player entity.
    if (entity != world.playerEntity) {
        return;
    }

    refresh(registry.get<Inventory>(entity));
}

void InventoryWindow::onItemUpdate(ItemID itemID)
{
    // If this update is for an item in the inventory, we need to refresh.
    const Item* item{itemData.getItem(itemID)};
    Inventory& inventory{world.registry.get<Inventory>(world.playerEntity)};
    auto it{std::find_if(inventory.slots.begin(), inventory.slots.end(),
                         [&](const Inventory::ItemSlot& itemSlot) {
                             return (itemSlot.ID == item->numericID);
                         })};
    if (it != inventory.slots.end()) {
        refresh(inventory);
    }
}

} // End namespace Client
} // End namespace AM
