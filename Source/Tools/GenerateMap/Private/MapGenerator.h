#pragma once

#include "NetworkDefs.h"
#include <string>
#include <cstdint>
#include <vector>

namespace AM
{
namespace MG
{
/**
 * Generates the TileMap.bin file based on the given parameters.
 */
class MapGenerator
{
public:
    MapGenerator(uint32_t inMapLengthX, uint32_t inMapLengthY,
                 const std::string& inFillSpriteSetId);

    /**
     * Generates the map and saves it to a file with the given name, placed in
     * the same directory as the program binary.
     */
    void generateAndSave(const std::string& fileName);

private:
    /** The version of the map format. Kept as just a 16-bit int for now, we
        can see later if we care to make it more complicated. */
    static constexpr uint16_t MAP_FORMAT_VERSION{1};

    /** The length, in chunks, of the map's X axis. */
    uint32_t mapXLength;

    /** The length, in chunks, of the map's Y axis. */
    uint32_t mapYLength;

    /** The ID of the sprite to fill the map with. */
    std::string fillSpriteSetID;
};

} // End namespace MG
} // End namespace AM
