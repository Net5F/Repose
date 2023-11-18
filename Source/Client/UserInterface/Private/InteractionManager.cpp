#include "InteractionManager.h"
#include "World.h"
#include "Network.h"
#include "MainScreen.h"
#include "Inventory.h"
#include "EntityInteractionRequest.h"
#include "UseItemOnEntityRequest.h"
#include "CombineItems.h"
#include "ItemInteractionRequest.h"
#include "InteractionHelpers.h"
#include "ItemThumbnail.h"
#include "Log.h"

namespace AM
{
namespace Client
{

InteractionManager::InteractionManager(World& inWorld, Network& inNetwork,
                                       MainScreen& inMainScreen)
: world{inWorld}
, network{inNetwork}
, mainScreen{inMainScreen}
, usingItem{false}
, sourceSlotIndex{0}
, sourceName{""}
{
}

void InteractionManager::entityHovered(entt::entity entity)
{
    // If this entity has no interactions, return early.
    auto* interaction{world.registry.try_get<Interaction>(entity)};
    if (!interaction || interaction->isEmpty()) {
        return;
    }
    Name& name{world.registry.get<Name>(entity)};

    // If we're using an item, update the text to reflect the hovered entity as 
    // the target.
    std::ostringstream stringStream{};
    if (usingItem) {
        stringStream << "Use " << sourceName << " on "
                     << name.value;
    }
    else {
        // Update the text to reflect the hovered entity's default interaction.
        EntityInteractionType defaultInteraction{interaction->getDefault()};
        stringStream << InteractionHelpers::toDisplayString(defaultInteraction)
                     << name.value;

        std::size_t interactionCount{interaction->getCount()};
        if (interactionCount > 1) {
            stringStream << " / " << std::to_string(interactionCount - 1)
                         << " more options.";
        }
    }

    onInteractionTextUpdated(stringStream.str());
}

void InteractionManager::entityLeftClicked(entt::entity entity)
{
    // If this entity has no interactions, return early.
    auto* interaction{world.registry.try_get<Interaction>(entity)};
    if (!interaction || interaction->isEmpty()) {
        return;
    }

    // If we're using an item, this is the target. Send the UseOn request and 
    // deselect the first item.
    if (usingItem) {
        EntityInteractionType defaultInteraction{
            interaction->supportedInteractions[0]};
        network.serializeAndSend(
            UseItemOnEntityRequest{sourceSlotIndex, entity});

        // Note: Dropping focus will cause the first item to deselect itself.
        mainScreen.dropFocus();
        usingItem = false;
    }
    else {
        // Not using an item. Request the default interaction be performed.
        network.serializeAndSend(
            EntityInteractionRequest{entity, interaction->getDefault()});
    }
}

void InteractionManager::entityRightClicked(entt::entity entity)
{
    if (auto* interaction{world.registry.try_get<Interaction>(entity)}) {
        if (interaction->isEmpty()) {
            // No interactions, return early.
            return;
        }

        // TODO: User right-clicked. Open the interaction menu.
    }
}

void InteractionManager::itemHovered(Uint8 slotIndex)
{
    // If the given slot doesn't contain an item, do nothing.
    auto& inventory{world.registry.get<Inventory>(world.playerEntity)};
    const Item* hoveredItem{inventory.getItem(slotIndex, world.itemData)};
    if (!hoveredItem) {
        return;
    }

    // If we're using an item, update the text to reflect the hovered item as 
    // the target.
    std::ostringstream stringStream{};
    if (usingItem) {
        stringStream << "Use " << sourceName << " on "
                     << hoveredItem->displayName;
    }
    else {
        // Update the text to reflect the hovered item's default interaction.
        ItemInteractionType defaultInteraction{
            hoveredItem->getDefaultInteraction()};
        stringStream << InteractionHelpers::toDisplayString(defaultInteraction)
                     << hoveredItem->displayName;

        std::size_t interactionCount{hoveredItem->getInteractionCount()};
        if (interactionCount > 1) {
            stringStream << " / " << std::to_string(interactionCount - 1)
                         << " more options.";
        }
    }

    onInteractionTextUpdated(stringStream.str());
}

void InteractionManager::itemLeftClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail)
{
    // If the given slot doesn't contain an item, do nothing.
    auto& inventory{world.registry.get<Inventory>(world.playerEntity)};
    const Item* clickedItem{inventory.getItem(slotIndex, world.itemData)};
    if (!clickedItem) {
        return;
    }

    // If we're using an item, this is the target. Send the combine request and 
    // deselect the first item.
    if (usingItem) {
        network.serializeAndSend(CombineItems{sourceSlotIndex, slotIndex});

        // Note: Dropping focus will cause the first item to deselect itself.
        mainScreen.dropFocus();
        usingItem = false;
    }
    else {
        // Not using an item. If this item's default interaction is UseOn, start
        // using it.
        ItemInteractionType defaultInteraction{
            clickedItem->getDefaultInteraction()};
        if (defaultInteraction == ItemInteractionType::UseOn) {
            sourceSlotIndex = slotIndex;
            sourceName = clickedItem->displayName;
            usingItem = true;
            mainScreen.setFocus(&itemThumbnail);

            std::ostringstream stringStream{};
            stringStream << "Use " << sourceName << " on";
            onInteractionTextUpdated(stringStream.str());
        }
        else {
            // Request the default interaction be performed.
            network.serializeAndSend(
                ItemInteractionRequest{slotIndex, defaultInteraction});
        }
    }
}

void InteractionManager::itemRightClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail)
{
    // TODO: Make sure we check for Destroy and send the different message
}

void InteractionManager::itemDeselected()
{
    if (usingItem) {
        // Item usage was canceled, reset our state and the text.
        usingItem = false;
        onInteractionTextUpdated("");
    }
}

void InteractionManager::unhovered()
{
    // If we're using an item, go back to the use text without a target.
    if (usingItem) {
        std::ostringstream stringStream{};
        stringStream << "Use " << sourceName << " on";
        onInteractionTextUpdated(stringStream.str());
    }
    else {
        // Not using an item, reset the text.
        onInteractionTextUpdated("");
    }
}

void InteractionManager::setOnInteractionTextUpdated(
    std::function<void(std::string_view)> inOnInteractionTextUpdated)
{
    onInteractionTextUpdated = std::move(inOnInteractionTextUpdated);
}

} // End namespace Client
} // End namespace AM
