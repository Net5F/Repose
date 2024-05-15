#include "MapGenerator.h"
#include "Paths.h"
#include "TileMapSnapshot.h"
#include "Serialize.h"
#include "Log.h"

namespace AM
{
namespace MG
{
MapGenerator::MapGenerator(uint16_t inMapLengthX, uint16_t inMapLengthY,
                           uint16_t inMapLengthZ, uint16_t inGroundLevel,
                           const std::string& inFillGraphicSetID,
                           uint8_t inFillGraphicIndex)
: mapXLength{inMapLengthX}
, mapYLength{inMapLengthY}
, mapZLength{inMapLengthZ}
, groundLevel{inGroundLevel}
, fillGraphicSetID{inFillGraphicSetID}
, fillGraphicIndex{inFillGraphicIndex}
{
}

void MapGenerator::generateAndSave(const std::string& fileName)
{
    // Fill the map's version and size.
    TileMapSnapshot tileMap{};
    tileMap.version = MAP_FORMAT_VERSION;
    tileMap.xLengthChunks = mapXLength;
    tileMap.yLengthChunks = mapYLength;
    tileMap.zLengthChunks = mapZLength;
    tileMap.chunks.resize(mapXLength * mapYLength * mapZLength);

    // Fill in the ground tiles.
    std::size_t groundLevelStartIndex{
        static_cast<std::size_t>(mapXLength * mapYLength * groundLevel)};
    for (std::size_t i{0}; i < (mapXLength * mapYLength); ++i) {
        ChunkSnapshot& chunk{tileMap.chunks[groundLevelStartIndex + i]};

        // Push the graphic that we're filling the map with into this chunk's 
        // palette.
        chunk.getPaletteIndex(TileLayer::Type::Floor, fillGraphicSetID,
                              fillGraphicIndex);

        // Push the palette index of the graphic into each tile.
        for (std::size_t j{0}; j < SharedConfig::CHUNK_TILE_COUNT; ++j) {
            chunk.tiles[j].layers.push_back(0);
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
