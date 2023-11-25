#include "InventoryWindow.h"
#include "Simulation.h"
#include "Network.h"
#include "InteractionManager.h"
#include "Paths.h"
#include "Inventory.h"
#include "ItemThumbnail.h"
#include "ItemInteractionRequest.h"
#include "InventoryDeleteItem.h"
#include "entt/entity/registry.hpp"

namespace AM
{
namespace Client
{
InventoryWindow::InventoryWindow(Simulation& inSimulation, Network& inNetwork,
                                 InteractionManager& inInteractionManager)
: AUI::Window({1362, 340, 248, 248}, "InventoryWindow")
, world{inSimulation.getWorld()}
, network{inNetwork}
, interactionManager{inInteractionManager}
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, itemContainer({12, 12, (logicalExtent.w - 12), (logicalExtent.h - 12)},
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
    // when an item defintion changes.
    world.registry.on_update<Inventory>()
        .connect<&InventoryWindow::onInventoryUpdated>(*this);
    inSimulation.getItemUpdatedSink().connect<&InventoryWindow::onItemUpdated>(
        *this);
}

AUI::EventResult
    InventoryWindow::onPreviewMouseDown(AUI::MouseButtonType buttonType,
                                        const SDL_Point& cursorPosition)
{
    // Give InteractionManager a chance to intercept this click.
    bool wasHandled{false};
    Uint8 slotIndex{0};
    for (std::unique_ptr<AUI::Widget>& item : itemContainer) {
        if (item->containsPoint(cursorPosition)) {
            wasHandled = interactionManager.itemPreviewMouseDown(
                slotIndex, buttonType,
                static_cast<ItemThumbnail&>(*(item.get())));
        }

        slotIndex++;
    }

    // If InteractionManager handled this click (i.e. if it was the second click
    // of a UseOn), handle it so it doesn't go on to select the thumbnail.
    return AUI::EventResult{.wasHandled{wasHandled}};
}

// TODO: Test with lots of items
//       Get rid of highlight, get rid of backdrop?
//       Final spacing
void InventoryWindow::refresh(const Inventory& inventory)
{
    // If an item was selected, deselect it.
    interactionManager.itemDeselected();

    // Add thumbnails for each item in the player's inventory.
    itemContainer.clear();
    for (std::size_t i = 0; i < inventory.items.size(); ++i) {
        // Skip empty slots.
        if (inventory.items[i].ID == NULL_ITEM_ID) {
            continue;
        }

        // Construct the new item thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<ItemThumbnail>(SDL_Rect{2, 2, 50, 50},
                                            "InventoryThumbnail")};
        ItemThumbnail& thumbnail{static_cast<ItemThumbnail&>(*thumbnailPtr)};

        // Load the item's images.
        thumbnail.backdropImage.setSimpleImage(
            Paths::TEXTURE_DIR + "Highlights/Hovered.png");
        thumbnail.thumbnailImage.setSimpleImage(
            Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Normal_1920.png");

        // Add our callbacks.
        Uint8 slotIndex{static_cast<Uint8>(i)};
        thumbnail.setOnHovered([&, slotIndex](ItemThumbnail* thumbnail) {
            interactionManager.itemHovered(slotIndex);
        });
        thumbnail.setOnUnhovered([&, slotIndex](ItemThumbnail* thumbnail) {
            interactionManager.unhovered();
        });
        thumbnail.setOnMouseDown(
            [&, slotIndex](ItemThumbnail* thumbnail,
                           AUI::MouseButtonType buttonType) {
            interactionManager.itemMouseDown(slotIndex, buttonType, *thumbnail);
        });
        thumbnail.setOnMouseUp([&, slotIndex](ItemThumbnail* thumbnail,
                                              AUI::MouseButtonType buttonType) {
            interactionManager.itemMouseUp(slotIndex, buttonType, *thumbnail);
        });
        thumbnail.setOnDeselected([&, slotIndex](ItemThumbnail* thumbnail) {
            interactionManager.itemDeselected();
        });

        itemContainer.push_back(std::move(thumbnailPtr));
    }
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

void InventoryWindow::onItemUpdated(const Item& item)
{
    // Item updates are only sent to players that own the item, so we already 
    // know it's in the inventory and we need to refresh.
    refresh(world.registry.get<Inventory>(world.playerEntity));
}

} // End namespace Client
} // End namespace AM
