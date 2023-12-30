#pragma once

#include "AUI/DragDropData.h"
#include <SDL_stdinc.h>
#include <variant>

namespace AM
{
namespace Client
{
/**
 * A derived type for passing data through drag and drop events.
 *
 * Currently only used for moving items in the inventory.
 */
struct DragDropData : public AUI::DragDropData {
    struct InventoryItem {
        /** The source inventory slot of the item that was dropped. */
        Uint8 sourceSlotIndex{0};
    };

    using DataType = std::variant<InventoryItem>;
    DragDropData(const DataType& inDataType)
    : AUI::DragDropData()
    , dataType{inDataType}
    {
    }

    /** The type of data that the dropped widget was carrying. */
    DataType dataType;
};

} // End namespace Client
} // End namespace AM
