#include "InteractionManager.h"
#include "World.h"
#include "Network.h"
#include "ItemData.h"
#include "MainScreen.h"
#include "ViewModel.h"
#include "Inventory.h"
#include "Interaction.h"
#include "CastRequest.h"
#include "UseItemOnEntityRequest.h"
#include "CombineItemsRequest.h"
#include "InventoryOperation.h"
#include "DisplayStrings.h"
#include "ItemThumbnail.h"
#include "Log.h"

namespace AM
{
namespace Client
{

InteractionManager::InteractionManager(World& inWorld, Network& inNetwork,
                                       const ItemData& inItemData,
                                       MainScreen& inMainScreen,
                                       const ViewModel& inViewModel)
: world{inWorld}
, network{inNetwork}
, itemData{inItemData}
, mainScreen{inMainScreen}
, viewModel{inViewModel}
, usingItem{false}
, sourceSlotIndex{0}
, sourceName{""}
{
}

void InteractionManager::entityLeftClicked(entt::entity entity)
{
    // If we're using an item, this is the target. Send the UseOn request and
    // deselect the first item.
    if (usingItem) {
        network.serializeAndSend(
            UseItemOnEntityRequest{sourceSlotIndex, entity});

        // Note: Dropping focus will cause the first item to deselect itself.
        mainScreen.dropFocus();
        usingItem = false;
    }
}

void InteractionManager::entityRightClicked(entt::entity entity)
{
    auto* interaction{world.registry.try_get<Interaction>(entity)};
    if (!interaction || interaction->supportedInteractions.empty()) {
        return;
    }

    // If we're using an item, cancel it.
    if (usingItem) {
        usingItem = false;
    }

    // Fill the right-click menu with this entity's interactions.
    mainScreen.clearRightClickMenu();
    for (EntityInteractionType interactionType :
         interaction->supportedInteractions) {
        // Tell the server to process this interaction.
        auto interactWith = [&, entity, interactionType]() {
            world.castHelper.queueEntityInteraction(
                {.interactionType{interactionType}, .targetEntity{entity}});
        };
        mainScreen.addRightClickMenuAction(DisplayStrings::get(interactionType),
                                           std::move(interactWith));
    }

    mainScreen.openRightClickMenu();
}

std::string InteractionManager::getItemTooltipString(Uint8 slotIndex)
{
    // If the given slot doesn't contain an item, do nothing.
    auto& inventory{world.registry.get<Inventory>(world.playerEntity)};
    const Item* hoveredItem{inventory.getItem(slotIndex, itemData)};
    if (!hoveredItem) {
        return "";
    }

    // If we're using an item, update the text to reflect the hovered item as
    // the target.
    std::string tooltipText{};
    if (usingItem) {
        tooltipText += "Use " + sourceName + " on " + hoveredItem->displayName;
    }
    // Else, update the text to reflect the hovered item's interactions.
    else {
        ItemInteractionType defaultInteraction{
            hoveredItem->getDefaultInteraction()};
        tooltipText += DisplayStrings::get(defaultInteraction) + " "
                       + hoveredItem->displayName;
    }

    return tooltipText;
}

bool InteractionManager::itemMouseDown(Uint8 slotIndex,
                                       AUI::MouseButtonType buttonType,
                                       ItemThumbnail& itemThumbnail)
{
    // There's no drag+drop on right click, so we can handle it immediately.
    if (buttonType == AUI::MouseButtonType::Right) {
        itemRightClicked(slotIndex, itemThumbnail);

        return false;
    }
    // We need to handle the 2nd click of UseOn immediately, so a drag+drop
    // doesn't get started.
    else if ((buttonType == AUI::MouseButtonType::Left) && usingItem
             && (slotIndex != sourceSlotIndex)) {
        // Send the combine request and reset our state.
        network.serializeAndSend(
            CombineItemsRequest{sourceSlotIndex, slotIndex});
        usingItem = false;

        return false;
    }
    else if (buttonType == AUI::MouseButtonType::Left) {
        // Nothing to do. Since we have drag+drop, we count MouseUp as the 
        // actual click. Drag behavior is handled by AUI.
    }

    // Request mouse capture, so we get the associated MouseUp.
    return true;
}

void InteractionManager::itemMouseUp(Uint8 slotIndex,
                                     AUI::MouseButtonType buttonType,
                                     ItemThumbnail& itemThumbnail)
{
    // Note: For items, since we have drag+drop, we count MouseUp as the actual
    //       click.

    if (buttonType == AUI::MouseButtonType::Left) {
        itemLeftClicked(slotIndex, itemThumbnail);
    }
}

void InteractionManager::itemDeselected()
{
    if (usingItem) {
        // Item usage was canceled, reset our state and the text.
        usingItem = false;
    }
}

void InteractionManager::itemLeftClicked(Uint8 slotIndex,
                                         ItemThumbnail& itemThumbnail)
{
    // If the given slot doesn't contain an item, do nothing.
    auto& inventory{world.registry.get<Inventory>(world.playerEntity)};
    const Item* clickedItem{inventory.getItem(slotIndex, itemData)};
    if (!clickedItem) {
        return;
    }

    // Note: The second click of UseOn is handled in itemMouseDown().

    // If this item's default interaction is UseOn, start using it.
    ItemInteractionType defaultInteraction{
        clickedItem->getDefaultInteraction()};
    if (defaultInteraction == ItemInteractionType::UseOn) {
        beginUseItemOnInteraction(slotIndex, clickedItem->displayName,
                                  itemThumbnail);
    }
    // Else, request the item's default interaction be performed.
    else {
        world.castHelper.queueItemInteraction(
            {.interactionType{defaultInteraction},
             .slotIndex{slotIndex},
             .targetEntity{viewModel.getTargetEntity()}});
    }
}

void InteractionManager::itemRightClicked(Uint8 slotIndex,
                                          ItemThumbnail& itemThumbnail)
{
    // If the given slot doesn't contain an item, do nothing.
    auto& inventory{world.registry.get<Inventory>(world.playerEntity)};
    const Item* clickedItem{inventory.getItem(slotIndex, itemData)};
    if (!clickedItem) {
        return;
    }

    // If we're using an item, cancel it.
    if (usingItem) {
        usingItem = false;
    }

    // Fill the right-click menu with this item's interactions.
    mainScreen.clearRightClickMenu();
    auto interactionList{clickedItem->getInteractionList()};
    for (ItemInteractionType interactionType : interactionList) {
        if (interactionType == ItemInteractionType::UseOn) {
            // If the item is still in the inventory, begin using it.
            auto useItemOn = [&, slotIndex, name{clickedItem->displayName},
                              thumbnail{AUI::WidgetWeakRef{itemThumbnail}}]() {
                if (thumbnail.isValid()) {
                    // Note: This will change focus, so we don't need to
                    //       explicitly drop focus to close the menu.
                    auto* thumbnailPtr{
                        static_cast<ItemThumbnail*>(&(thumbnail.get()))};
                    beginUseItemOnInteraction(slotIndex, name, *thumbnailPtr);
                }
            };
            mainScreen.addRightClickMenuAction("Use", std::move(useItemOn));
        }
        else if (interactionType == ItemInteractionType::Destroy) {
            // Tell the server to remove the items in this slot.
            auto deleteItem = [&, slotIndex,
                               count{inventory.slots[slotIndex].count}]() {
                network.serializeAndSend(
                    InventoryOperation{InventoryRemoveItem{slotIndex, count}});
            };
            mainScreen.addRightClickMenuAction("Destroy",
                                               std::move(deleteItem));
        }
        else {
            // Tell the sim to process this interaction.
            auto interactWith = [&, slotIndex, interactionType]() {
                world.castHelper.queueItemInteraction(
                    {.interactionType{interactionType},
                     .slotIndex{slotIndex},
                     .targetEntity{viewModel.getTargetEntity()}});
            };
            mainScreen.addRightClickMenuAction(
                DisplayStrings::get(interactionType), std::move(interactWith));
        }
    }

    mainScreen.openRightClickMenu();
}

void InteractionManager::beginUseItemOnInteraction(Uint8 slotIndex,
                                                   std::string_view displayName,
                                                   ItemThumbnail& itemThumbnail)
{
    sourceSlotIndex = slotIndex;
    sourceName = displayName;
    usingItem = true;

    // Note: We leave thumbnails as non-focusable most of the time, since we
    //       only want them to be focused when we Use them.
    //       InventoryWindow handles setting isFocusable back to false when
    //       they lose focus.
    itemThumbnail.setIsFocusable(true);
    mainScreen.setFocus(&itemThumbnail);
}

} // End namespace Client
} // End namespace AM
