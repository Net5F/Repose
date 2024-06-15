#pragma once

#include <string>
#include <cstdint>

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
    MapGenerator(uint16_t inMapLengthX, uint16_t inMapLengthY,
                 uint16_t inMapLengthZ, uint16_t inGroundLevel,
                 const std::string& inFillGraphicSetID);

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
    uint16_t mapXLength;

    /** The length, in chunks, of the map's Y axis. */
    uint16_t mapYLength;

    /** The length, in chunks, of the map's Z axis. */
    uint16_t mapZLength;

    /** The Z axis value where the ground should be placed. */
    uint16_t groundLevel;

    /** The ID of the graphic set to fill the map with. */
    std::string fillGraphicSetID;
};

} // End namespace MG
} // End namespace AM
