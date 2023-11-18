#pragma once

#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/VerticalListContainer.h"
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
                    InteractionManager& inInteractionManager);

private:
    /**
     * Refreshes this widget, making it match the given inventory.
     */
    void refresh(const Inventory& inventory);

    void onInventoryUpdated(entt::registry& registry, entt::entity entity);
    void onItemUpdated(const Item& item);

    /** Used to interact with the player's inventory. */
    World& world;
    /** Used to send interaction requests. */
    Network& network;
    /** Used to orchestrate item/entity interactions. */
    InteractionManager& interactionManager;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    /** Holds the inventory items. */
    AUI::VerticalListContainer itemContainer;
};

} // End namespace Client
} // End namespace AM
