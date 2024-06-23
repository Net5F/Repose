#include "MapGenerator.h"
#include "Paths.h"
#include "TileMapSnapshot.h"
#include "ChunkExtent.h"
#include "Serialize.h"
#include "Log.h"

namespace AM
{
namespace MG
{
MapGenerator::MapGenerator(uint16_t inMapLengthX, uint16_t inMapLengthY,
                           uint16_t inMapLengthZ, uint16_t inGroundLevel,
                           const std::string& inFillGraphicSetID)
: mapXLength{inMapLengthX}
, mapYLength{inMapLengthY}
, mapZLength{inMapLengthZ}
, groundLevel{inGroundLevel}
, fillGraphicSetID{inFillGraphicSetID}
{
}

void MapGenerator::generateAndSave(const std::string& fileName)
{
    // Fill the map's version and size.
    TileMapSnapshot mapSnapshot{};
    mapSnapshot.version = MAP_FORMAT_VERSION;
    mapSnapshot.xLengthChunks = mapXLength;
    mapSnapshot.yLengthChunks = mapYLength;
    mapSnapshot.zLengthChunks = mapZLength;

    // Fill in the ground tiles.
    ChunkExtent mapChunkExtent{
        ChunkExtent::fromMapLengths(mapXLength, mapYLength, mapZLength)};
    for (int x{mapChunkExtent.x}; x <= mapChunkExtent.xMax(); ++x) {
        for (int y{mapChunkExtent.y}; y <= mapChunkExtent.yMax(); ++y) {
            ChunkPosition chunkPosition{x, y, groundLevel};
            ChunkSnapshot& chunkSnapshot{mapSnapshot.chunks[chunkPosition]};

            // Push the graphic that we're filling the map with into this chunk's 
            // palette.
            chunkSnapshot.getPaletteIndex(TileLayer::Type::Terrain,
                                          fillGraphicSetID, 0);

            // Push the palette index of the graphic into each tile.
            for (std::size_t i{0}; i < SharedConfig::CHUNK_TILE_COUNT;
                 ++i) {
                chunkSnapshot.tileLayerCounts[i] = 1;
                chunkSnapshot.tileLayers.push_back(0);
            }
        }
    }

    // Serialize the map snapshot and write it to the file.
    bool saveSuccessful{
        Serialize::toFile((Paths::BASE_PATH + fileName), mapSnapshot)};
    if (!saveSuccessful) {
        LOG_FATAL("Failed to serialize and save the map.");
    }
}

} // End namespace MG
} // End namespace AM
