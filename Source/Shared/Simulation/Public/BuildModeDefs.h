#pragma once

#include "TileExtent.h"

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
const TileExtent BUILD_MODE_AREA_EXTENT{55, 17, 7, 12};

/**
 * The extent of the "valid build area": the room where we allow clients to 
 * modify tiles.
 */
const TileExtent VALID_BUILD_AREA_EXTENT{56, 17, 6, 12};

} // End namespace AM
