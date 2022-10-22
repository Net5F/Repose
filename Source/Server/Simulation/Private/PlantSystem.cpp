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
{
    // Add the regions.
    plantRegions.emplace_back();
    plantRegions.emplace_back();

    PlantRegion& garden{plantRegions[0]};
    garden.extent = {5, 36, 8, 11};
    garden.plantTypes = {Plant::Type::Rose, Plant::Type::Hydrangea};

    PlantRegion& jungle{plantRegions[1]};
    jungle.extent = {55, 17, 7, 12};
    jungle.plantTypes = {Plant::Type::TreeFern, Plant::Type::BananaLeafPlant};

    // Initialize the open tiles for each region.
    for (int x = garden.extent.x; x <= garden.extent.xMax(); ++x) {
        for (int y = garden.extent.y; y <= garden.extent.yMax(); ++y) {
            garden.openTiles.emplace_back(x, y);
        }
    }

    for (int x = jungle.extent.x; x <= jungle.extent.xMax(); ++x) {
        for (int y = jungle.extent.y; y <= jungle.extent.yMax(); ++y) {
            jungle.openTiles.emplace_back(x, y);
        }
    }

    // Initialize the plant counts for each region.
    const int gardenPlantCount{10};
    for (std::size_t i = 0; i < gardenPlantCount; ++i) {
        garden.plants.emplace_back();
    }

    const int junglePlantCount{14};
    for (std::size_t i = 0; i < junglePlantCount; ++i) {
        jungle.plants.emplace_back();
    }

    // Initialize all the plants.
    for (PlantRegion& region : plantRegions) {
        // Clear any existing plant sprites.
        world.tileMap.clearExtent(region.extent, 1);

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
        unsigned int currentStage{
            static_cast<unsigned int>(plant.lifeStage)};
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
        0, region.openTiles.size()};
    auto openTileIt{region.openTiles.begin() + openTileDist(generator)};
    plant.position = *openTileIt;

    std::swap(*openTileIt, *(region.openTiles.end() - 1));
    region.openTiles.pop_back();

    // Randomize the plant's type.
    std::uniform_int_distribution<std::size_t> typeDist{
        0, region.plantTypes.size()};
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
        case Plant::Type::Rose: {
            switch (plant.lifeStage) {
                case Plant::LifeStage::Sapling:
                    return spriteData.get("gap_fill_0").numericID;
                case Plant::LifeStage::MidGrowth:
                    return spriteData.get("gap_fill_1").numericID;
                case Plant::LifeStage::FullyGrown:
                    return spriteData.get("gap_fill_2").numericID;
                case Plant::LifeStage::Dying:
                    return spriteData.get("gap_fill_3").numericID;
                default:
                    return spriteData.get("empty").numericID;
            }
            break;
        }
        case Plant::Type::Hydrangea: {
            switch (plant.lifeStage) {
                case Plant::LifeStage::Sapling:
                    return spriteData.get("gap_fill_0").numericID;
                case Plant::LifeStage::MidGrowth:
                    return spriteData.get("gap_fill_1").numericID;
                case Plant::LifeStage::FullyGrown:
                    return spriteData.get("gap_fill_2").numericID;
                case Plant::LifeStage::Dying:
                    return spriteData.get("gap_fill_3").numericID;
                default:
                    return spriteData.get("empty").numericID;
            }
            break;
        }
        case Plant::Type::TreeFern: {
            switch (plant.lifeStage) {
                case Plant::LifeStage::Sapling:
                    return spriteData.get("gap_fill_0").numericID;
                case Plant::LifeStage::MidGrowth:
                    return spriteData.get("gap_fill_1").numericID;
                case Plant::LifeStage::FullyGrown:
                    return spriteData.get("gap_fill_2").numericID;
                case Plant::LifeStage::Dying:
                    return spriteData.get("gap_fill_3").numericID;
                default:
                    return spriteData.get("empty").numericID;
            }
            break;
        }
        case Plant::Type::BananaLeafPlant: {
            switch (plant.lifeStage) {
                case Plant::LifeStage::Sapling:
                    return spriteData.get("gap_fill_0").numericID;
                case Plant::LifeStage::MidGrowth:
                    return spriteData.get("gap_fill_1").numericID;
                case Plant::LifeStage::FullyGrown:
                    return spriteData.get("gap_fill_2").numericID;
                case Plant::LifeStage::Dying:
                    return spriteData.get("gap_fill_3").numericID;
                default:
                    return spriteData.get("empty").numericID;
            }
            break;
        }
        default: {
            return spriteData.get("empty").numericID;
        }
    }
}

} // End namespace Server
} // End namespace AM
