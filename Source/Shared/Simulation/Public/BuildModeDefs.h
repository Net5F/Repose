#pragma once

#include "TileExtent.h"

/**
 * This file contains shared build mode-relevant definitions that should be
 * consistent between the server and client.
 */
namespace AM
{

/**
 * The extent of the "build area": the room where we allow clients to modify 
 * tiles.
 */
const TileExtent BUILD_AREA_EXTENT{55, 17, 7, 12};

} // End namespace AM
