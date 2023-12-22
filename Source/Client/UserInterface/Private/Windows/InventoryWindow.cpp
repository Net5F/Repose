#include "InventoryWindow.h"
#include "Simulation.h"
#include "Network.h"
#include "IconData.h"
#include "InteractionManager.h"
#include "Paths.h"
#include "Inventory.h"
#include "ItemThumbnail.h"
#include "ItemInteractionRequest.h"
#include "AUI/ScalingHelpers.h"
#include "entt/entity/registry.hpp"

namespace AM
{
namespace Client
{
InventoryWindow::InventoryWindow(Simulation& inSimulation, Network& inNetwork,
                                 IconData& inIconData,
                                 InteractionManager& inInteractionManager)
: AUI::Window({1362, 340, 248, 248}, "InventoryWindow")
, world{inSimulation.getWorld()}
, network{inNetwork}
, iconData{inIconData}
, interactionManager{inInteractionManager}
, wasRefreshed{false}
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, itemContainer({12, 12, (logicalExtent.w - 12), (logicalExtent.h - 18)},
                "ItemContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(itemContainer);

    /* Background image */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});

    /* Item container */
    itemContainer.setNumColumns(4);
    itemContainer.setCellWidth(58);
    itemContainer.setCellHeight(58);

    // We need to update this widget when the player's inventory changes, or 
    // when an item definition changes.
    world.registry.on_update<Inventory>()
        .connect<&InventoryWindow::onInventoryUpdated>(*this);
    inSimulation.getItemUpdateSink().connect<&InventoryWindow::onItemUpdate>(
        *this);
}

void InventoryWindow::updateLayout()
{
    // Do the normal layout updating.
    Window::updateLayout();

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
        for (Uint8 slotIndex = 0; slotIndex < itemContainer.size(); ++slotIndex) {
            std::unique_ptr<AUI::Widget>& item{itemContainer[slotIndex]};
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

    // Add thumbnails for each item in the player's inventory.
    itemContainer.clear();
    for (Uint8 slotIndex = 0; slotIndex < inventory.slots.size(); ++slotIndex) {
        // Fill empty slots with a blank, hidden image to preserve spacing.
        ItemID itemID{inventory.slots[slotIndex].ID};
        if (itemID == NULL_ITEM_ID) {
            itemContainer.push_back(std::make_unique<AUI::Image>(
                SDL_Rect{0, 0, 50, 50}, "InventoryBlankSpace"));
            itemContainer.back()->setIsVisible(false);
            continue;
        }

        // Construct the new item thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<ItemThumbnail>(SDL_Rect{0, 0, 50, 50},
                                            "InventoryThumbnail")};
        ItemThumbnail& thumbnail{static_cast<ItemThumbnail&>(*thumbnailPtr)};

        // Load the item's icon.
        if (const Item* item{world.itemData.getItem(itemID)}) {
            const IconRenderData& renderData{
                iconData.getRenderData(item->iconID)};

            thumbnail.thumbnailImage.setSimpleImage(renderData.iconSheetRelPath,
                                                    renderData.textureExtent);
        }
        else {
            // Default icon
            thumbnail.thumbnailImage.setSimpleImage(
                Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Normal_1920.png");
        }

        // Load the thumbnail's images.
        // Load the thumbnail's selected image.
        thumbnail.selectedImage.setNineSliceImage(
            Paths::TEXTURE_DIR + "Thumbnail/Selected_NineSlice.png",
            {4, 4, 4, 4});

        // Add our callbacks.
        thumbnail.setOnHovered([&, slotIndex](ItemThumbnail* thumbnail) {
            interactionManager.itemHovered(slotIndex);
        });
        thumbnail.setOnUnhovered([&, slotIndex](ItemThumbnail* thumbnail) {
            interactionManager.unhovered();
        });
        thumbnail.setOnMouseDown(
            [&, slotIndex](ItemThumbnail* thumbnail,
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

        itemContainer.push_back(std::move(thumbnailPtr));
    }

    wasRefreshed = true;
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

void InventoryWindow::onItemUpdate(const Item& item)
{
    // If this update is for an item in the inventory, we need to refresh.
    Inventory& inventory{world.registry.get<Inventory>(world.playerEntity)};
    auto it{std::find_if(inventory.slots.begin(), inventory.slots.end(),
                         [&](const Inventory::ItemSlot& itemSlot) {
                             return (itemSlot.ID == item.numericID);
                         })};
    if (it != inventory.slots.end()) {
        refresh(inventory);
    }
}

} // End namespace Client
} // End namespace AM
