#pragma once

#include "InteractionRequest.h"
#include "TileExtent.h"
#include "Timer.h"
#include "entt/fwd.hpp"
#include <queue>
#include <random>

namespace AM
{
struct Position;
struct ObjectSpriteSet;

namespace Server
{

class Simulation;
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
    PlantSystem(Simulation& inSimulation, SpriteData& inSpriteData);

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
    void replantPlant(entt::entity oldPlant);

    /** Used to modify the tile map. */
    World& world;
    /** Used to get the plant sprites that we apply to the tile map. */
    SpriteData& spriteData;

    Timer updateTimer;

    // Queue for receiving interaction events.
    std::queue<InteractionRequest> replantInteractionQueue;

    /** The world extent that the plants are located within. */
    TileExtent plantExtent;

    Uint16 sunflowerSpriteSetID;
};

} // End namespace Server
} // End namespace AM
