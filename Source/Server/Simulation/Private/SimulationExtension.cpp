#include "SimulationExtension.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "TileExtent.h"
#include "BuildModeDefs.h"
#include "SharedConfig.h"
#include "Log.h"

namespace AM
{
namespace Server
{

SimulationExtension::SimulationExtension(SimulationExDependencies deps)
: buildModeDataSystem{deps.world, deps.network.getEventDispatcher(),
                        deps.network, deps.spriteData}
, mazeGenerationSystem{deps.world, deps.spriteData}
, plantSystem{deps.world, deps.spriteData}
, teleportSystem{deps.world}
{
}

void SimulationExtension::beforeAll() {}

void SimulationExtension::afterMapAndConnectionUpdates()
{
    // Respond to any build mode data messages that aren't handled 
    // by the engine.
    buildModeDataSystem.processMessages();

    // Regenerate the maze, if enough time has passed.
    mazeGenerationSystem.regenerateMazeIfNecessary();

    // Update any plants that are ready for growth or replanting.
    plantSystem.updatePlants();
}

void SimulationExtension::afterMovement()
{
    // Teleport any players that are touching a teleport volume.
    teleportSystem.teleportPlayers();
}

void SimulationExtension::afterMovementSync() {}

bool SimulationExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

bool SimulationExtension::isExtentEditable(const TileExtent& tileExtent)
{
    if (SharedConfig::RESTRICT_TILE_UPDATES) {
        // Only return true for updates within the build area.
        return VALID_BUILD_AREA_EXTENT.containsExtent(tileExtent);
    }
    else {
        // No restrictions, always return true;
        return true;
    }
}

} // End namespace Server
} // End namespace AM
