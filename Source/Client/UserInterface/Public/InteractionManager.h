#pragma once

#include "entt/fwd.hpp"
#include <SDL_stdinc.h>
#include <functional>
#include <string_view>
#include <string>

namespace AM
{
namespace Client
{
class World;
class Network;
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
                       MainScreen& inMainScreen);

    // Entity interactions.
    void entityHovered(entt::entity entity);
    void entityLeftClicked(entt::entity entity);
    void entityRightClicked(entt::entity entity);

    // Item interactions.
    /** @param slotIndex The inventory slot of the item. */
    void itemHovered(Uint8 slotIndex);
    void itemLeftClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail);
    void itemRightClicked(Uint8 slotIndex, ItemThumbnail& itemThumbnail);
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
    /** Used to access the registry for interactions and inventory. */
    World& world;
    /** Used to send interaction messages. */
    Network& network;
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
