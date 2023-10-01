#include "RendererExtension.h"
#include "RendererExDependencies.h"
#include "Camera.h"
#include "Log.h"

namespace AM
{
namespace Client
{

RendererExtension::RendererExtension(
    [[maybe_unused]] const RendererExDependencies& deps)
{
}

void RendererExtension::beforeWorld([[maybe_unused]] const Camera& lerpedCamera,
                                    [[maybe_unused]] double alpha)
{
}

void RendererExtension::afterWorld([[maybe_unused]] const Camera& lerpedCamera,
                                   [[maybe_unused]] double alpha)
{
}

bool RendererExtension::handleOSEvent([[maybe_unused]] SDL_Event& event)
{
    return false;
}

} // End namespace Client
} // End namespace AM
