#include "InventoryWindow.h"
#include "Network.h"
#include "Paths.h"
#include "Ignore.h"

namespace AM
{
namespace Client
{
InventoryWindow::InventoryWindow(Network& inNetwork)
: AUI::Window({1362, 340, 248, 248}, "InventoryWindow")
, network{inNetwork}
, systemMessageQueue{inNetwork.getEventDispatcher()}
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, itemContainer({0, 0, logicalExtent.w, logicalExtent.h}, "ItemContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(itemContainer);

    /* Background image */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
}

void InventoryWindow::onTick(double)
{
    // Process any waiting messages.
    SystemMessage systemMessage{};
    while (systemMessageQueue.pop(systemMessage)) {
    }
}

} // End namespace Client
} // End namespace AM
