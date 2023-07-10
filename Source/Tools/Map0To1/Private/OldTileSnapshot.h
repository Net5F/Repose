#pragma once

#include <SDL_stdinc.h>
#include <vector>

namespace AM
{
/**
 * Holds tile data in a persistable form (palette IDs instead of pointers).
 *
 * Used in saving/loading the tile map and in sending map data over the
 * network.
 */
struct OldTileSnapshot {
public:
    /** Used as a "we should never hit this" cap on the number of layers in a
        tile. Only checked in debug builds. */
    static constexpr unsigned int MAX_SPRITE_LAYERS = 100;

    /** The layers of sprites that make up this tile, stored bottom to top.

        Sprites are referred to by their index in the palette of the chunk that
        contains this tile. */
    std::vector<Uint8> spriteLayers;
};

template<typename S>
void serialize(S& serializer, OldTileSnapshot& testTile)
{
    serializer.container1b(testTile.spriteLayers,
                           OldTileSnapshot::MAX_SPRITE_LAYERS);
}

} // End namespace AM