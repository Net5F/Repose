#pragma once

#include "Timer.h"
#include <SDL_stdinc.h>

namespace AM
{
namespace Server
{

/**
 * The data associated with a plant entity.
 */
struct Plant {
    /**
     * The stages of life that a plant goes through.
     * As the plant progresses through stages 0 - 3, it will switch to the
     * sprite at the same index within the entity's current sprite set.
     * E.g. when a plant progress to MidGrowth, it will switch to the sprite
     * at spriteSet.sprites[1].
     */
    enum class LifeStage : Uint8 {
        Sapling,
        MidGrowth,
        FullyGrown,
        Dead
    };

    /** How long it takes for this plant to progress to the next stage, in
        seconds. */
    static constexpr float UPDATE_TIME_S{10};

    /** This plant's current stage of life. */
    LifeStage lifeStage{LifeStage::Sapling};

    /** Tracks how much time has passed since this plant's last update. */
    Timer timer{};
};

template<typename S>
void serialize(S& serializer, Plant& plant)
{
    serializer.value1b(plant.lifeStage);
    // Note: We purposely don't serialize timer, since it wouldn't make sense to.
}

} // End namespace Server
} // End namespace AM
