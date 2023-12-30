#pragma once

#include "ItemID.h"
#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/VerticalGridContainer.h"
#include "QueuedEvents.h"
#include "entt/fwd.hpp"

namespace AM
{
struct Inventory;
struct Item;

namespace Client
{
class Simulation;
class World;
class Network;
class IconData;
class InteractionManager;
class ItemThumbnail;

/**
 * The inventory window on the main screen. Shows the player's inventory.
 */
class InventoryWindow : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    InventoryWindow(Simulation& inSimulation, Network& inNetwork,
                    IconData& inIconData,
                    InteractionManager& inInteractionManager);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    /**
     * Calls Window::updateLayout() and, if itemContainer has been refreshed,
     * notifies InteractionManager.
     */
    void updateLayout() override;

private:
    /** The height and width of our thumbnail widgets, in logical pixels. */
    static constexpr int THUMBNAIL_SIZE{58};

    /**
     * Refreshes itemContainer, making it match the given inventory.
     */
    void refresh(const Inventory& inventory);

    /**
     * Adds an empty thumbnail for the given slot.
     */
    ItemThumbnail& addEmptyThumbnail(Uint8 slotIndex);

    /**
     * Turns the given empty thumbnail into an item thumbnail.
     * Used for non-empty slots, after first calling addEmptyThumbnail().
     */
    void finishItemThumbnail(ItemThumbnail& thumbnail, ItemID itemID,
                             Uint8 slotIndex);

    void onInventoryUpdated(entt::registry& registry, entt::entity entity);
    void onItemUpdate(const Item& item);

    /** Used to interact with the player's inventory. */
    World& world;
    /** Used to send interaction requests. */
    Network& network;
    /** Used to get item icons. */
    IconData& iconData;
    /** Used to orchestrate item/entity interactions. */
    InteractionManager& interactionManager;

    /** If true, itemContainer has been refreshed and InteractionManager needs
        to be notified. */
    bool wasRefreshed;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    /** Holds the inventory slot thumbnails. */
    AUI::VerticalGridContainer slotContainer;
};

} // End namespace Client
} // End namespace AM
