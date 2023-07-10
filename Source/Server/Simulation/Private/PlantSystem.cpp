#include "PlantSystem.h"
#include "World.h"
#include "SpriteData.h"
#include "EmptySpriteID.h"
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
, SUNFLOWER_SPRITE_SET_ID{spriteData.getObjectSpriteSet("sunflower").numericID}
, SUNFLOWER_SAPLING_INDEX{0}
, SUNFLOWER_MIDGROWTH_INDEX{1}
, SUNFLOWER_FULLYGROWN_INDEX{2}
, SUNFLOWER_DYING_INDEX{3}
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
        world.tileMap.clearExtentLayers<ObjectTileLayer>(region.extent);

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
    if (plant.timer.getTime() > updateTimeS) {
        // Update the plant to the next stage.
        Uint8 oldStage{static_cast<Uint8>(plant.lifeStage)};
        Uint8 newStage{static_cast<Uint8>(oldStage + 1)};
        plant.lifeStage = static_cast<Plant::LifeStage>(newStage);

        // If the plant is dead, replant it.
        if (plant.lifeStage == Plant::LifeStage::Dead) {
            replantPlant(region, plant);
        }
        else {
            // Else, update the plant's sprite to reflect the new life stage.
            world.tileMap.remObject(plant.position.x, plant.position.y,
                                    SUNFLOWER_SPRITE_SET_ID,
                                    static_cast<Rotation::Direction>(oldStage));
            world.tileMap.addObject(plant.position.x, plant.position.y,
                                    SUNFLOWER_SPRITE_SET_ID,
                                    static_cast<Rotation::Direction>(newStage));
        }

        plant.timer.reset();
    }
}

void PlantSystem::replantPlant(PlantRegion& region, Plant& plant)
{
    // If the plant was previously initialized.
    if ((plant.position.x != -1) && (plant.position.y != -1)) {
        // Remove the sprite from the old position.
        world.tileMap.remObject(
            plant.position.x, plant.position.y, SUNFLOWER_SPRITE_SET_ID,
            static_cast<Rotation::Direction>(Plant::LifeStage::Dying));

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
    world.tileMap.addObject(
        plant.position.x, plant.position.y, SUNFLOWER_SPRITE_SET_ID,
        static_cast<Rotation::Direction>(Plant::LifeStage::Sapling));
}

} // End namespace Server
} // End namespace AM
