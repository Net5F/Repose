#pragma once

#include "MazeCell.h"

namespace AM
{
namespace Server
{

/**
 * Holds the topology of a maze.
 * 
 * Note: Since we want maze hallways to be 2 cells wide, we typically generate 
 *       the maze at half size and double it before applying to the tile map.
 */
struct MazeTopology
{
    std::vector<MazeCell> cells{};
};

} // End namespace Server
} // End namespace AM
