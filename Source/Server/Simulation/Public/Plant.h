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
        Sunflower,
    };

    /**
     * The stages of life that a plant goes through.
     * The values of stages 0 - 3 line up with their sprite's index within the 
     * sunflower sprite set.
     */
    enum class LifeStage : unsigned int {
        Sapling,
        MidGrowth,
        FullyGrown,
        Dying,
        Dead
    };

    /** The tile that this plant resides on. */
    TilePosition position{-1, -1};

    /** The type of plant that this is. */
    Type type{Type::Sunflower};

    /** How long it takes for this plant to progress to the next stage, in
        seconds. */
    double updateTimeS{0};

    /** This plant's current stage of life. */
    LifeStage lifeStage{LifeStage::Sapling};

    /** Tracks how much time has passed since this plant's last update. */
    Timer timer{};
};

} // End namespace Server
} // End namespace AM
