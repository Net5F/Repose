#pragma once

#include "TilePosition.h"
#include "Timer.h"

namespace AM
{
namespace Server
{

/**
 * A single plant.
 */
struct Plant {
    /**
     * Possible types of plant.
     */
    enum class Type : unsigned int {
        Rose = 0,
        Hydrangea,
        TreeFern,
        BananaLeafPlant
    };

    /**
     * The stages of life that a plant goes through.
     */
    enum class LifeStage : unsigned int {
        Sapling = 0,
        MidGrowth,
        FullyGrown,
        Dying,
        Dead
    };

    /** The tile that this plant resides on. */
    TilePosition position{-1, -1};

    /** The type of plant that this is. */
    Type type{Type::Rose};

    /** How long it takes for this plant to progress to the next stage, in 
        seconds. */
    double updateTimeS{0};

    /** This plant's current stage of life. */
    LifeStage lifeStage{LifeStage::Sapling};

    /** Tracks how much time has passed since this plant's last update. */
    Timer timer;
};

} // End namespace Server
} // End namespace AM
