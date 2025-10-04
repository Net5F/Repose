#pragma once

#include "TileExtent.h"
#include "ItemID.h"
#include <array>

/**
 * This file contains shared build mode-relevant definitions that should be
 * consistent between the server and client.
 */
namespace AM
{

/**
 * The extent of the "build mode area": the area where the client will be able
 * to open their build mode UI.
 * We keep this slightly larger than the actual build area, so a bad actor
 * can't wall-off the build area.
 */
const std::array<TileExtent, 3> BUILD_MODE_AREA_EXTENTS{
    TileExtent{21, 39, 0, 9, 8, 1}, TileExtent{30, 30, 0, 8, 9, 1},
    TileExtent{38, 39, 0, 9, 8, 1}};

/**
 * The extent of the "valid build area": the room where we allow clients to
 * modify tiles.
 */
const std::array<TileExtent, 3> VALID_BUILD_AREA_EXTENTS{
    TileExtent{21, 39, 0, 8, 8, 1}, TileExtent{30, 30, 0, 8, 8, 1},
    TileExtent{39, 39, 0, 8, 8, 1}};

/**
 * The items in this list are not allowed to be edited by clients.
 */
const std::vector<ItemID> PROTECTED_ITEMS{
    1, // TestItem
    2, // CombineItem1
    3, // CombineItem2
    4  // ResultItem
};

} // End namespace AM
