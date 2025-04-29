#pragma once

#include "GraphicSetIDs.h"
#include "TileExtent.h"
#include "Timer.h"
#include "entt/fwd.hpp"
#include <random>
#include <array>

namespace AM
{
struct Position;
struct ObjectGraphicSet;
struct CastInfo;

namespace Server
{

class World;
class GraphicData;

/**
 * Takes plants through their growth, death, and replanting life cycle.
 *
 * Plant growth is managed in 2 specific regions: the garden, and the jungle.
 */
class PlantSystem
{
public:
    PlantSystem(World& inWorld, GraphicData& inGraphicData);

    /**
     * Iterates through the plants and updates any that need to grow or die.
     */
    void updatePlants();

private:
    /** How often to check if plants need to be updated. */
    static constexpr double UPDATE_TIMESTEP_S{0.2};

    /**
     * Updates the given plant entity to the next stage of life.
     */
    void updatePlant(entt::entity plantEntity);

    /**
     * Creates a new entity with a Plant component set to Sapling.
     */
    void createSapling(const Position& position);

    /**
     * Creates a new entity with a Plant component set to Dead, and a
     * "Replant" interaction.
     */
    void createDeadPlant(const Position& position);

    /**
     * Deletes the given old plant and plants a new sapling in its place.
     */
    void replantPlant(const CastInfo& castInfo);

    /** Used to modify the tile map. */
    World& world;
    /** Used to get the plant graphics that we apply to the tile map. */
    GraphicData& graphicData;

    Timer updateTimer;

    /** The world extent that the plants are located within. */
    TileExtent plantExtent;

    /** IDs for: Sapling, MidGrowth, FullyGrown, and Dead sunflower states.
        Indices match LifeStage enum. */
    std::array<EntityGraphicSetID, 4> sunflowerGraphicSetIDs;
};

} // End namespace Server
} // End namespace AM
