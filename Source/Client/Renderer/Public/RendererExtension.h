#pragma once

#include "IRendererExtension.h"
#include "RendererExDependencies.h"

namespace AM
{
namespace Client
{

/**
 * PLACEHOLDER - Currently not rendering anything at the project level.
 */
class RendererExtension : public IRendererExtension
{
public:
    // Note: This is the canonical constructor, expected by the factory that
    //       constructs this class. Do not modify it.
    RendererExtension(RendererExDependencies deps);

    /**
     * Called before tiles and entities are rendered.
     */
    void beforeWorld(const Camera& lerpedCamera, double alpha) override;

    /**
     * Called after tiles and entities are rendered.
     */
    void afterWorld(const Camera& lerpedCamera, double alpha) override;

    /**
     * See OSEventHandler for details.
     *
     * Note: Renderer will pass events to this class first. If the event is
     *       not handled, then Renderer will attempt to handle it.
     */
    bool handleOSEvent(SDL_Event& event) override;

private:
};

} // End namespace Client
} // End namespace AM
