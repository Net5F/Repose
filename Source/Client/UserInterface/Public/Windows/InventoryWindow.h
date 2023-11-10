#pragma once

#include "SystemMessage.h"
#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/VerticalListContainer.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Client
{
class Network;

/**
 * The inventory window on the main screen. Shows the player's inventory.
 */
class InventoryWindow : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    InventoryWindow(Network& inNetwork);

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

private:
    /** Used to send interaction requests. */
    Network& network;

    EventQueue<SystemMessage> systemMessageQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    /** Holds the inventory items. */
    AUI::VerticalListContainer itemContainer;
};

} // End namespace Client
} // End namespace AM
