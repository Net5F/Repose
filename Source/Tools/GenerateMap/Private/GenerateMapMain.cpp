#include "MapGenerator.h"
#include "Timer.h"
#include "Ignore.h"

#include <cstdint>
#include <iostream>

using namespace AM;
using namespace AM::MG;

void printUsage()
{
    std::printf("Usage: GenerateMap.exe <XLength> <YLength> <FillSpriteSetId> "
                "<FillSpriteIndex>\n"
                "  XLength: The map's x-axis length in chunks.\n"
                "  YLength: The map's y-axis length in chunks.\n"
                "  FillSpriteSetId: The string ID of the floor sprite set to "
                "fill the map with.\n");
    std::fflush(stdout);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::printf("Invalid arguments.\n");
        printUsage();
        return 1;
    }

    // Parse map X length.
    char* end;
    int mapLengthX{std::strtol(argv[1], &end, 10)};
    if ((*end != '\0') || (mapLengthX < 1) || (mapLengthX > SDL_MAX_UINT32)) {
        // Input didn't parse into an integer, or was an invalid number.
        std::printf("Invalid XLength: %s\n", argv[1]);
        printUsage();
        return 1;
    }

    // Parse map Y length.
    int mapLengthY{std::strtol(argv[2], &end, 10)};
    if ((*end != '\0') || (mapLengthY < 1) || (mapLengthY > SDL_MAX_UINT32)) {
        // Input didn't parse into an integer, or was an invalid number.
        std::printf("Invalid YLength: %s\n", argv[2]);
        printUsage();
        return 1;
    }

    // Parse fill sprite ID.
    std::string fillSpriteId{argv[3]};

    // Generate the map and save it.
    Timer timer;
    MapGenerator mapGenerator(static_cast<uint32_t>(mapLengthX),
                              static_cast<uint32_t>(mapLengthY), fillSpriteId);
    mapGenerator.generateAndSave("TileMap.bin");

    double timeTaken{timer.getTime()};
    std::printf("Map generated and saved in %.6fs.\n", timeTaken);
    std::fflush(stdout);

    return 0;
}
