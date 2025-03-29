#pragma once

#include "AUI/MouseButtonType.h"
#include "AUI/WidgetWeakRef.h"
#include "entt/fwd.hpp"
#include <SDL_stdinc.h>
#include <SDL_rect.h>
#include <functional>
#include <string_view>
#include <string>

namespace AM
{
namespace Client
{
class World;
class Network;
class ItemData;
class MainScreen;
class ItemThumbnail;

/**
 * Manages entity and item interactions, and the interaction text in the top
 * left corner.
 *
 * MainOverlay and InventoryWindow report to this class when the mouse hovers
 * over or clicks an interactable entity or item. This class then tells
 * MainOverlay what to display in the interaction text, and sends an interaction
 * message if appropriate.
 */
class InteractionManager
{
public:
    InteractionManager(World& inWorld, Network& inNetwork,
                       const ItemData& inItemData, MainScreen& inMainScreen);

    // Entity interactions.
    /** @param entity The relevant entity. */
    void entityHovered(entt::entity entity);
    void entityLeftClicked(entt::entity entity);
    void entityRightClicked(entt::entity entity);

    // Item interactions.
    /** @param slotIndex The inventory slot containing the relevant item.
        @param itemThumbnail The widget to focus. */
    void itemHovered(Uint8 slotIndex);
    /** @return true if the given thumbnail should request mouse capture, else
                false. */
    bool itemMouseDown(Uint8 slotIndex, AUI::MouseButtonType buttonType,
                       ItemThumbnail& itemThumbnail);
    void itemMouseUp(Uint8 slotIndex, AUI::MouseButtonType buttonType,
                     ItemThumbnail& itemThumbnail);
    void itemDeselected();

    /**
     * Called when the mouse moves away from hovering over an entity or item.
     */
    void unhovered();

    /**
     * Sets a callback for when our interaction state changes and the text
     * should be updated.
     */
    void setOnInteractionTextUpdated(
        std::function<void(std::string_view)> inOnInteractionTextUpdated);

private:
    void itemLeftClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail);
    void itemRightClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail);

    /**
     * Begins a "Use item on" interaction.
     * If there's an existing item being used, replaces it with the new item.
     *
     * @param slotIndex The item's inventory slot containing the item that's
     *                  being used.
     * @param displayName The item's display name.
     * @param itemThumbnail The thumbnail of the item that's being used.
     */
    void beginUseItemOnInteraction(Uint8 slotIndex,
                                   std::string_view displayName,
                                   ItemThumbnail& itemThumbnail);

    /** Used to access the registry for interactions and inventory. */
    World& world;
    /** Used to send interaction messages. */
    Network& network;
    /** Used to access item data. */
    const ItemData& itemData;
    /** Used to set focus to a selected item. */
    MainScreen& mainScreen;

    /** If true, there's an ongoing "Use item on X" interaction. */
    bool usingItem;

    /** If usingItem == true, this is the inventory slot of the item that is
        being used. */
    Uint8 sourceSlotIndex;

    /** If usingItem == true, this is the display name of the item that is
        being used. */
    std::string sourceName;

    std::function<void(std::string_view)> onInteractionTextUpdated;
};

} // namespace Client
} // namespace AM
