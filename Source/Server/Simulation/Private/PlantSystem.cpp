#include "PlantSystem.h"
#include "World.h"
#include "SpriteData.h"
#include "Log.h"
#include "AMAssert.h"

namespace AM
{
namespace Server
{

PlantSystem::PlantSystem(World& inWorld, SpriteData& inSpriteData)
: world{inWorld}
, spriteData{inSpriteData}
, plantRegions{}
, randomDevice()
, generator(randomDevice())
, SUNFLOWER_SAPLING_ID{spriteData.get("sunflower_0").numericID}
, SUNFLOWER_MIDGROWTH_ID{spriteData.get("sunflower_1").numericID}
, SUNFLOWER_FULLYGROWN_ID{spriteData.get("sunflower_2").numericID}
, SUNFLOWER_DYING_ID{spriteData.get("sunflower_3").numericID}
{
    // Add the regions.
    plantRegions.emplace_back();

    PlantRegion& field{plantRegions[0]};
    field.extent = {5, 36, 8, 11};
    field.plantTypes = {Plant::Type::Sunflower};

    // Initialize the open tiles for each region.
    for (int x = field.extent.x; x <= field.extent.xMax(); ++x) {
        for (int y = field.extent.y; y <= field.extent.yMax(); ++y) {
            field.openTiles.emplace_back(x, y);
        }
    }

    // Initialize the plant counts for each region.
    const int fieldPlantCount{20};
    for (std::size_t i = 0; i < fieldPlantCount; ++i) {
        field.plants.emplace_back();
    }

    // Initialize all the plants.
    for (PlantRegion& region : plantRegions) {
        // Clear any existing plant sprites.
        world.tileMap.clearExtentSpriteLayers(region.extent, 1, 1);

        // Randomize the parameters for every plant and place their sprites.
        for (Plant& plant : region.plants) {
            replantPlant(region, plant);
        }
    }
}

void PlantSystem::updatePlants()
{
    // Update all the plants.
    for (PlantRegion& region : plantRegions) {
        for (Plant& plant : region.plants) {
            updatePlant(region, plant);
        }
    }
}

void PlantSystem::updatePlant(PlantRegion& region, Plant& plant)
{
    // Keep the plant in the fully grown stage longer than the other stages.
    double updateTimeS{plant.updateTimeS};
    if (plant.lifeStage == Plant::LifeStage::FullyGrown) {
        updateTimeS *= 3;
    }

    // If the plant needs to be updated.
    if (plant.timer.getDeltaSeconds(false) > updateTimeS) {
        // Update the plant to the next stage.
        unsigned int currentStage{static_cast<unsigned int>(plant.lifeStage)};
        currentStage++;
        plant.lifeStage = static_cast<Plant::LifeStage>(currentStage);

        // If the plant is dead, replant it.
        if (plant.lifeStage == Plant::LifeStage::Dead) {
            replantPlant(region, plant);
        }
        else {
            // Else, update the plant's sprite to reflect the new life stage.
            world.tileMap.setTileSpriteLayer(plant.position.x, plant.position.y,
                                             1, getPlantSpriteID(plant));
        }

        plant.timer.updateSavedTime();
    }
}

void PlantSystem::replantPlant(PlantRegion& region, Plant& plant)
{
    // If the plant was previously initialized.
    if ((plant.position.x != -1) && (plant.position.y != -1)) {
        // Remove the sprite from the old position.
        world.tileMap.setTileSpriteLayer(plant.position.x, plant.position.y, 1,
                                         spriteData.get("empty"));

        // Return the old position to the pool.
        region.openTiles.push_back(plant.position);
    }

    // Pull a random position from the pool.
    std::uniform_int_distribution<std::size_t> openTileDist{
        0, (region.openTiles.size() - 1)};
    auto openTileIt{region.openTiles.begin() + openTileDist(generator)};
    plant.position = *openTileIt;

    AM_ASSERT(region.openTiles.size() > 0,
              "openTiles vector was empty (should never happen).");
    std::swap(*openTileIt, *(region.openTiles.end() - 1));
    region.openTiles.pop_back();

    // Randomize the plant's type.
    std::uniform_int_distribution<std::size_t> typeDist{
        0, (region.plantTypes.size() - 1)};
    plant.type = region.plantTypes[typeDist(generator)];

    // Randomize the plant's updateTime.
    std::uniform_real_distribution<float> timeDist{MINIMUM_UPDATE_TIME_S,
                                                   MAXIMUM_UPDATE_TIME_S};
    plant.updateTimeS = timeDist(generator);

    // Set the plant back to the Sapling stage.
    plant.lifeStage = Plant::LifeStage::Sapling;

    // Set the plant's new sprite.
    world.tileMap.setTileSpriteLayer(plant.position.x, plant.position.y, 1,
                                     getPlantSpriteID(plant));
}

int PlantSystem::getPlantSpriteID(Plant& plant)
{
    switch (plant.type) {
        case Plant::Type::Sunflower: {
            switch (plant.lifeStage) {
                case Plant::LifeStage::Sapling: {
                    return SUNFLOWER_SAPLING_ID;
                }
                case Plant::LifeStage::MidGrowth: {
                    return SUNFLOWER_MIDGROWTH_ID;
                }
                case Plant::LifeStage::FullyGrown: {
                    return SUNFLOWER_FULLYGROWN_ID;
                }
                case Plant::LifeStage::Dying: {
                    return SUNFLOWER_DYING_ID;
                }
                default: {
                    LOG_INFO("Empty 1");
                    return spriteData.get("empty").numericID;
                }
            }
            break;
        }
        default: {
            LOG_INFO("Empty 2: %u, %u", static_cast<unsigned>(plant.type),
                     static_cast<unsigned>(plant.lifeStage));
            return spriteData.get("empty").numericID;
        }
    }
}

} // End namespace Server
} // End namespace AM
