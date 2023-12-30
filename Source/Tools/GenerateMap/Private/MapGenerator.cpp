#include "MapGenerator.h"
#include "ByteTools.h"
#include "Paths.h"
#include "SharedConfig.h"
#include "TileMapSnapshot.h"
#include "Serialize.h"
#include <iostream>
#include <cstring>
#include <fstream>

namespace AM
{
namespace MG
{
MapGenerator::MapGenerator(uint32_t inMapLengthX, uint32_t inMapLengthY,
                           const std::string& inFillSpriteSetID)
: mapXLength{inMapLengthX}
, mapYLength{inMapLengthY}
, fillSpriteSetID{inFillSpriteSetID}
{
}

void MapGenerator::generateAndSave(const std::string& fileName)
{
    // Fill the map's version and size.
    TileMapSnapshot tileMap;
    tileMap.version = MAP_FORMAT_VERSION;
    tileMap.xLengthChunks = mapXLength;
    tileMap.yLengthChunks = mapYLength;

    // Fill the chunks.
    tileMap.chunks.resize(mapXLength * mapYLength);
    for (ChunkSnapshot& chunk : tileMap.chunks) {
        // Push the sprite set ID that we're filling the map with into the
        // palette.
        chunk.getPaletteIndex(TileLayer::Type::Floor, fillSpriteSetID, 0);

        // Push the palette index of the sprite into each tile.
        for (std::size_t i = 0; i < SharedConfig::CHUNK_TILE_COUNT; ++i) {
            chunk.tiles[i].layers.push_back(0);
        }
    }

    // Serialize the map snapshot and write it to the file.
    bool saveSuccessful{
        Serialize::toFile((Paths::BASE_PATH + fileName), tileMap)};
    if (!saveSuccessful) {
        LOG_FATAL("Failed to serialize and save the map.");
    }
}

} // End namespace MG
} // End namespace AM
