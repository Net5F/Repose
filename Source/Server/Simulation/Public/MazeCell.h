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

    /** Tracks which generation pass this cell was last visited during.
        If -1, this cell has not been visited. */
    int lastVisitedPassNumber{-1};
};

} // End namespace Server
} // End namespace AM
