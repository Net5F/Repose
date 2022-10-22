#pragma once

#include "PlantRegion.h"
#include "HashTools.h"
#include <random>

namespace AM
{
namespace Server
{

class World;
class SpriteData;

/**
 * Takes plants through their growth, death, and replanting life cycle.
 *
 * Plant growth is managed in 2 specific regions: the garden, and the jungle.
 */
class PlantSystem
{
public:
    PlantSystem(World& inWorld, SpriteData& inSpriteData);

    /**
     * Iterates through the plants and updates any that need to grow or die.
     */
    void updatePlants();

private:
    /** The minimum time a plant can take before updating. */
    static constexpr float MINIMUM_UPDATE_TIME_S{5};

    /** The maximum time a plant can take before updating. */
    static constexpr float MAXIMUM_UPDATE_TIME_S{30};

    /**
     * Updates the given plant to the next stage of life.
     * If the plant dies, calls replantPlant().
     */
    void updatePlant(PlantRegion& region, Plant& plant);

    /**
     * Replants the given plant, re-randomizing its parameters and setting it 
     * back to LifeStage::Sapling.
     */
    void replantPlant(PlantRegion& region, Plant& plant);

    /**
     * Returns the sprite ID to display for the given plant's type and stage.
     */
    int getPlantSpriteID(Plant& plant);

    /** Used to modify the tile map. */
    World& world;

    /** Used to get the plant sprites that we apply to the tile map. */
    SpriteData& spriteData;

    /** The regions where plants are being grown. */
    std::vector<PlantRegion> plantRegions;

    // For random positions, types, and update times.
    std::random_device randomDevice;
    std::mt19937 generator;
};

} // End namespace Server
} // End namespace AM
