#pragma once

#include "TileExtent.h"
#include "Plant.h"

namespace AM
{
namespace Server
{

/**
 * A region where certain types of plants will grow.
 */
struct PlantRegion {
    /** The region's extent. */
    TileExtent extent{};

    /** Holds all the tiles that are available for planting. */
    std::vector<TilePosition> openTiles;

    /** The types of plant that can grow in this region. */
    std::vector<Plant::Type> plantTypes;

    /** The plants that currently exist in this region. */
    std::vector<Plant> plants;
};

} // End namespace Server
} // End namespace AM
