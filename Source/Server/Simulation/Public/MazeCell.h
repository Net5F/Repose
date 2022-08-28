#pragma once

#include "MazeCell.h"

namespace AM
{
namespace Server
{

/**
 * A single cell in the maze.
 * 
 * Note: We only place north and west walls. East walls are placed by putting a
 *       west wall on the cell to the east, and south walls are placed by 
 *       putting a north wall on the cell to the south.
 */
struct MazeCell
{
    /** If true, the wall is present. */
    bool northWall{true};
    bool westWall{true};

    /** If true, this cell has been visited. */
    bool wasVisited{false};
};

} // End namespace Server
} // End namespace AM
