#include "TileMapSnapshot.h"
#include "Serialize.h"
#include "Deserialize.h"
#include "Log.h"

#include <fstream>
#include <string>

using namespace AM;

void printUsage()
{
    std::printf("Usage: ReplaceMapSpriteID.exe <TileMapPath> "
                "<SpriteIDToReplace> <NewSpriteID>\n"
                "  Finds all occurences of SpriteIDToReplace in the given map "
                "and replaces them with NewSpriteID.\n");
    std::fflush(stdout);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::printf("Too few arguments.\n");
        printUsage();
        return 1;
    }

    // Try to deserialize the map file into a snapshot.
    TileMapSnapshot mapSnapshot;
    std::string filePath{argv[1]};
    if (!Deserialize::fromFile(filePath, mapSnapshot)) {
        LOG_FATAL("Failed to deserialize map at path: %s", filePath.c_str());
    }

    // Find and replace all occurrences.
    std::string spriteIDToReplace{argv[2]};
    std::string newSpriteID{argv[3]};
    int replaceCount{0};
    for (ChunkSnapshot& chunk : mapSnapshot.chunks) {
        for (std::string& spriteID : chunk.palette) {
            if (spriteID == spriteIDToReplace) {
                spriteID = newSpriteID;
                replaceCount++;
            }
        }
    }

    // If we replaced any occurrences, try to save the map.
    if (replaceCount > 0) {
        bool saveSuccessful{Serialize::toFile(filePath, mapSnapshot)};
        if (saveSuccessful) {
            LOG_INFO("Found and replaced the given ID in %d palettes.",
                     replaceCount);
        }
        else {
            LOG_INFO("Failed to serialize and save the map.");
        }
    }
    else {
        LOG_INFO("The given ID was not found in the given map.");
    }

    return 0;
}
