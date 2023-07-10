#include "TileMapSnapshot.h"
#include "OldTileMapSnapshot.h"
#include "Serialize.h"
#include "Deserialize.h"
#include "Log.h"

#include <fstream>
#include <string>

using namespace AM;

int tileX{0};
int tileY{0};
int chunkX{0};
int chunkY{0};
ChunkSnapshot::PaletteEntry
    oldSpriteToNewPaletteEntry(const std::string& oldSpriteID)
{
    // Floors
    if (oldSpriteID == "floor0") {
        return {TileLayer::Type::Floor, "grass0", 0};
    }
    else if (oldSpriteID == "floor1") {
        return {TileLayer::Type::Floor, "grass1", 0};
    }
    else if (oldSpriteID == "floor2") {
        return {TileLayer::Type::Floor, "grass2", 0};
    }
    else if (oldSpriteID == "floor3") {
        return {TileLayer::Type::Floor, "grass3", 0};
    }
    else if (oldSpriteID == "floor4") {
        return {TileLayer::Type::Floor, "grass4", 0};
    }
    else if (oldSpriteID == "floor5") {
        return {TileLayer::Type::Floor, "grass5", 0};
    }
    // Floor Coverings
    else if (oldSpriteID == "grass0") {
        return {TileLayer::Type::FloorCovering, "grass", 0};
    }
    else if (oldSpriteID == "grass1") {
        return {TileLayer::Type::FloorCovering, "grass", 1};
    }
    else if (oldSpriteID == "grass2") {
        return {TileLayer::Type::FloorCovering, "grass", 2};
    }
    else if (oldSpriteID == "grass3") {
        return {TileLayer::Type::FloorCovering, "grass", 3};
    }
    else if (oldSpriteID == "grass4") {
        return {TileLayer::Type::FloorCovering, "grass", 4};
    }
    // Objects
    else if (oldSpriteID == "grass_west1") {
        return {TileLayer::Type::Object, "grass", 0};
    }
    else if (oldSpriteID == "grass_north1") {
        return {TileLayer::Type::Object, "grass", 2};
    }
    else if (oldSpriteID == "grass_nw1") {
        return {TileLayer::Type::Object, "grass", 4};
    }
    else if (oldSpriteID == "grass_nw2") {
        return {TileLayer::Type::Object, "grass", 5};
    }
    else if (oldSpriteID == "grass_north2") {
        return {TileLayer::Type::Object, "grass", 3};
    }
    else if (oldSpriteID == "grass_west2") {
        return {TileLayer::Type::Object, "grass", 1};
    }
    else if (oldSpriteID == "sunflower_0") {
        return {TileLayer::Type::Object, "sunflower", 0};
    }
    else if (oldSpriteID == "sunflower_1") {
        return {TileLayer::Type::Object, "sunflower", 1};
    }
    else if (oldSpriteID == "sunflower_2") {
        return {TileLayer::Type::Object, "sunflower", 2};
    }
    else if (oldSpriteID == "sunflower_3") {
        return {TileLayer::Type::Object, "sunflower", 3};
    }
    else if (oldSpriteID == "fullfill1") {
        return {TileLayer::Type::Object, "hedgefullfill", 0};
    }
    else if (oldSpriteID == "fullfill2") {
        return {TileLayer::Type::Object, "hedgefullfill", 1};
    }
    else if (oldSpriteID == "fullfill3") {
        return {TileLayer::Type::Object, "hedgefullfill", 2};
    }
    // Walls
    else if (oldSpriteID == "ne_fill1") {
        LOG_INFO("NE fill: Tile(%d, %d)", chunkX * 16 + tileX, chunkY * 16 + tileY);
        return {TileLayer::Type::Wall, "squaredhedge", 3};
    }
    else if (oldSpriteID == "ne_fill2") {
        return {TileLayer::Type::Wall, "squaredhedgeflower", 3};
    }
    else if (oldSpriteID == "ne_fill3") {
        return {TileLayer::Type::Wall, "hedgeflower", 3};
    }
    else if (oldSpriteID == "west3") {
        return {TileLayer::Type::Wall, "hedge", 0};
    }
    else if (oldSpriteID == "nw_fill1") {
        return {TileLayer::Type::Wall, "hedge", 2};
    }
    else if (oldSpriteID == "nw_fill2") {
        return {TileLayer::Type::Wall, "roundedhedgeflower", 2};
    }
    else if (oldSpriteID == "nw_fill3") {
        return {TileLayer::Type::Wall, "squaredhedgeflower", 2};
    }
    else if (oldSpriteID == "north1") {
        return {TileLayer::Type::Wall, "roundedhedge", 1};
    }
    else if (oldSpriteID == "north2") {
        return {TileLayer::Type::Wall, "roundedhedgeflower", 1};
    }
    else if (oldSpriteID == "west4") {
        return {TileLayer::Type::Wall, "hedgeflower", 0};
    }
    else if (oldSpriteID == "north3") {
        return {TileLayer::Type::Wall, "hedge", 1};
    }
    else if (oldSpriteID == "north4") {
        return {TileLayer::Type::Wall, "hedgeflower", 1};
    }
    else if (oldSpriteID == "north5") {
        LOG_INFO("Square North: Tile(%d, %d)", chunkX * 16 + tileX, chunkY * 16 + tileY);
        return {TileLayer::Type::Wall, "squaredhedge", 1};
    }
    else if (oldSpriteID == "north6") {
        return {TileLayer::Type::Wall, "squaredhedgeflower", 1};
    }
    else if (oldSpriteID == "west5") {
        return {TileLayer::Type::Wall, "squaredhedge", 0};
    }
    else if (oldSpriteID == "west1") {
        return {TileLayer::Type::Wall, "roundedhedge", 0};
    }
    else if (oldSpriteID == "west2") {
        return {TileLayer::Type::Wall, "roundedhedgeflower", 0};
    }
    else if (oldSpriteID == "west5") {
        return {TileLayer::Type::Wall, "squaredhedge", 0};
    }
    else if (oldSpriteID == "west6") {
        return {TileLayer::Type::Wall, "squaredhedgeflower", 0};
    }
    // Template
    else if (oldSpriteID == "template_floor") {
        return {TileLayer::Type::Floor, "template", 0};
    }
    else if (oldSpriteID == "template_fullfill") {
        return {TileLayer::Type::Object, "templatefullfill", 0};
    }
    else if (oldSpriteID == "template_ne_fill") {
        return {TileLayer::Type::Wall, "template", 3};
    }
    else if (oldSpriteID == "template_nw_fill") {
        return {TileLayer::Type::Wall, "template", 2};
    }
    else if (oldSpriteID == "template_north") {
        return {TileLayer::Type::Wall, "template", 1};
    }
    else if (oldSpriteID == "template_west") {
        return {TileLayer::Type::Wall, "template", 0};
    }
    // Roberto
    else if (oldSpriteID == "roberto") {
        return {TileLayer::Type::Object, "roberto", 0};
    }
    // Ghost
    else if (oldSpriteID == "ghost_east") {
        return {TileLayer::Type::Object, "ghost", 6};
    }
    else if (oldSpriteID == "ghost_north") {
        return {TileLayer::Type::Object, "ghost", 4};
    }
    else if (oldSpriteID == "ghost_south") {
        return {TileLayer::Type::Object, "ghost", 0};
    }
    else if (oldSpriteID == "ghost_west") {
        return {TileLayer::Type::Object, "ghost", 2};
    }
    // Random
    else if (oldSpriteID == "column") {
        return {TileLayer::Type::Object, "column", 0};
    }
    else {
        LOG_FATAL("Sprite string ID not found: %s", oldSpriteID.c_str());
        return {};
    }
}

void convertOldTileToNew(const std::vector<std::string>& oldPalette,
                         const OldTileSnapshot& oldTile,
                         ChunkSnapshot& newChunk, TileSnapshot& newTile)
{
    // Add each of the old tile's layers to the new tile.
    for (Uint8 paletteIndex : oldTile.spriteLayers) {
        const std::string oldSpriteID{oldPalette[paletteIndex]};

        // Skip empty layers.
        if (oldSpriteID == "empty" || oldSpriteID == "ghost5") {
            continue;
        }

        // Get the new palette entry info for the old sprite.
        ChunkSnapshot::PaletteEntry paletteEntry{
            oldSpriteToNewPaletteEntry(oldSpriteID)};

        // Filter out the errant wall.
        if (chunkX * 16 + tileX == 27 && chunkY * 16 + tileY == 54) {
            if (paletteEntry.layerType == TileLayer::Type::Wall
                && paletteEntry.spriteIndex == Wall::Type::NorthEastGapFill) {
                continue;
            }
        }

        // Add the entry to the palette.
        std::size_t paletteIndex{newChunk.getPaletteIndex(
            paletteEntry.layerType, paletteEntry.spriteSetID,
            paletteEntry.spriteIndex)};

        // Add the palette index to the tile.
        newTile.layers.push_back(static_cast<Uint8>(paletteIndex));
    }
}

int main(int argc, char* argv[])
{
    // Arg[1]: Old TileMap.bin
    // Arg[2]: New TileMap.bin
    if (argc != 3) {
        std::printf("Too few arguments.\n");
        return 1;
    }

    // Try to deserialize the map file into a snapshot.
    OldTileMapSnapshot oldMapSnapshot;
    std::string oldFilePath{argv[1]};
    if (!Deserialize::fromFile(oldFilePath, oldMapSnapshot)) {
        LOG_FATAL("Failed to deserialize map at path: %s", oldFilePath.c_str());
    }

    // Fill the new map.
    TileMapSnapshot newMapSnapshot{};
    newMapSnapshot.version = 1;
    newMapSnapshot.xLengthChunks = oldMapSnapshot.xLengthChunks;
    newMapSnapshot.yLengthChunks = oldMapSnapshot.yLengthChunks;
    for (const OldChunkSnapshot& chunk : oldMapSnapshot.chunks) {
        // Add a chunk to the new map.
        newMapSnapshot.chunks.emplace_back();

        tileX = 0;
        tileY = 0;
        // Fill the chunk's tiles.
        for (std::size_t i = 0; i < chunk.tiles.size(); ++i) {
            convertOldTileToNew(chunk.palette, chunk.tiles[i],
                                newMapSnapshot.chunks.back(),
                                newMapSnapshot.chunks.back().tiles[i]);
            tileX++;
            if (tileX == 16) {
                tileX = 0;
                tileY++;
            }
        }
        chunkX++;
        if (chunkX == 4) {
            chunkX = 0;
            chunkY++;
        }
    }

    std::string newFilePath{argv[2]};
    bool saveSuccessful{Serialize::toFile(newFilePath, newMapSnapshot)};
    if (saveSuccessful) {
        LOG_INFO("Successfully serialized and saved map.");
    }
    else {
        LOG_INFO("Failed to serialize and save the map.");
    }

    return 0;
}
