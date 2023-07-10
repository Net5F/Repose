#pragma once

#include "OldTileSnapshot.h"
#include "SharedConfig.h"
#include <vector>
#include <array>
#include <string>

namespace AM
{
/**
 * Holds chunk data in a persistable form (palette IDs instead of pointers).
 *
 * Used in saving/loading the tile map.
 */
struct OldChunkSnapshot {
public:
    /** Used as a "we should never hit this" cap on the number of IDs in a
        palette. Only checked in debug builds. */
    static constexpr unsigned int MAX_IDS = 1000;

    /** Used as a "we should never hit this" cap on the size of each ID string
        in the palette. Only checked in debug builds. */
    static constexpr unsigned int MAX_ID_LENGTH = 50;

    /** Holds the string IDs of all the sprites used in this chunk's tiles.
        Tile layers hold indices into this palette. */
    std::vector<std::string> palette;

    /** The tiles that make up this chunk, stored in row-major order. */
    std::array<OldTileSnapshot, SharedConfig::CHUNK_TILE_COUNT> tiles;

    /**
     * Returns the palette index for the given ID.
     * If the ID is not in the palette, it will be added.
     */
    unsigned int getPaletteIndex(const std::string& stringID)
    {
        // TODO: If this gets to be a performance issue, we can look into
        //       switching palette to a map. Serialization will be more
        //       complicated, though.

        // Check if we already have this string.
        for (unsigned int i = 0; i < palette.size(); ++i) {
            if (palette[i] == stringID) {
                // We already have the string, returns its index.
                return i;
            }
        }

        // We didn't have the string, add it.
        palette.push_back(stringID);
        return static_cast<unsigned int>(palette.size() - 1);
    }
};

template<typename S>
void serialize(S& serializer, OldChunkSnapshot& testChunk)
{
    serializer.container(testChunk.palette, OldChunkSnapshot::MAX_IDS,
                         [](S& serializer, std::string& string) {
                             serializer.text1b(string,
                                               OldChunkSnapshot::MAX_ID_LENGTH);
                         });

    serializer.container(testChunk.tiles);
}

} // End namespace AM