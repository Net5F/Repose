#include "MapGenerator.h"
#include "Timer.h"

#include <cstdint>
#include <iostream>

using namespace AM;
using namespace AM::MG;

const int MAX_X_LENGTH{64};
const int MAX_Y_LENGTH{64};
const int MAX_Z_LENGTH{10};

int main(int, char**)
{
    std::printf("##################################\n");
    std::printf("## Amalgam Engine Map Generator ##\n");
    std::printf("##################################\n");
    std::string inputBuffer{};

    int mapLengthX{};
    while (1) {
        std::printf("\nX axis length? (chunks)\n");
        std::printf("Valid values: 1 - %d, whole numbers\n", MAX_X_LENGTH);
        inputBuffer.clear();
        std::cin >> inputBuffer;

        char* end;
        mapLengthX = std::strtol(inputBuffer.c_str(), &end, 10);
        if ((*end != '\0') || (mapLengthX < 1) || (mapLengthX > MAX_X_LENGTH)) {
            std::printf( "Invalid value.\n");
        }
        else {
            break;
        }
    }

    int mapLengthY{};
    while (1) {
        std::printf("\nY axis length? (chunks)\n");
        std::printf("Valid values: 1 - %d, whole numbers\n", MAX_Y_LENGTH);
        inputBuffer.clear();
        std::cin >> inputBuffer;

        char* end;
        mapLengthY = std::strtol(inputBuffer.c_str(), &end, 10);
        if ((*end != '\0') || (mapLengthY < 1) || (mapLengthY > MAX_Y_LENGTH)) {
            std::printf("Invalid value.\n");
        }
        else {
            break;
        }
    }

    int mapLengthZ{};
    while (1) {
        std::printf("\nZ axis length? (chunks)\n");
        std::printf("Valid values: 1 - %d, whole numbers\n", MAX_Z_LENGTH);
        inputBuffer.clear();
        std::cin >> inputBuffer;

        char* end;
        mapLengthZ = std::strtol(inputBuffer.c_str(), &end, 10);
        if ((*end != '\0') || (mapLengthZ < 1) || (mapLengthZ > MAX_Z_LENGTH)) {
            std::printf("Invalid value.\n");
        }
        else {
            break;
        }
    }

    int groundLevel{};
    while (1) {
        std::printf("\nWhere along the Z axis should the ground be placed?\n");
        std::printf("Valid values: 0 - %d, whole numbers\n", (mapLengthZ - 1));
        inputBuffer.clear();
        std::cin >> inputBuffer;

        char* end;
        groundLevel = std::strtol(inputBuffer.c_str(), &end, 10);
        if ((*end != '\0') || (groundLevel < 0)
            || (groundLevel > (mapLengthZ - 1))) {
            std::printf("Invalid value.\n");
        }
        else {
            break;
        }
    }

    std::string fillGraphicSetID{""};
    std::printf("\nWhich graphic set should be used for the ground?\n");
    std::printf("Valid values: any graphic set string ID\n");
    inputBuffer.clear();
    std::cin >> fillGraphicSetID;

    // Generate the map and save it.
    Timer timer{};
    MapGenerator mapGenerator(
        static_cast<uint16_t>(mapLengthX), static_cast<uint16_t>(mapLengthY),
        static_cast<uint16_t>(mapLengthZ), static_cast<uint16_t>(groundLevel),
        fillGraphicSetID);
    mapGenerator.generateAndSave("TileMap.bin");

    double timeTaken{timer.getTime()};
    std::printf("Map generated and saved in %.6fs.\n", timeTaken);
    std::fflush(stdout);

    return 0;
}
