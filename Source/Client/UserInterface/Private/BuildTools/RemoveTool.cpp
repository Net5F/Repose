#include "RemoveTool.h"
#include "WorldObjectLocator.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "Ignore.h"

namespace AM
{
namespace Client 
{
struct WorldObjectVisitor
{
    void operator()(std::monostate) const { LOG_INFO("Monostate"); }

    void operator()(const TileLayerID& layerID) const
    {
        LOG_INFO("Layer type: %u", layerID.type);
    }

    void operator()(entt::entity entity) const
    {
        LOG_INFO("Entity ID: %u", entity);
    }
};

RemoveTool::RemoveTool(const World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       EventDispatcher& inUiEventDispatcher)
: BuildTool(inWorld, inUiEventDispatcher)
, worldObjectLocator{inWorldObjectLocator}
{
}

void RemoveTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().
    ignore(cursorPosition);

    // If this tool is active and the user left clicked.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
        WorldObjectID objectID{
            worldObjectLocator.getObjectUnderPoint(cursorPosition)};
        std::visit(WorldObjectVisitor{}, objectID);
    }
}

void RemoveTool::onMouseUp(AUI::MouseButtonType buttonType,
                          const SDL_Point& cursorPosition)
{
    // TODO: Add support for click-and-drag to remove an extent instead of a 
    //       single layer.
}

void RemoveTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void RemoveTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // If this tool is active.
    if (isActive) {
        // Check if the mouse is over an object.
        SDL_Point offsetScreenPosition{cursorPosition};
        offsetScreenPosition.x += static_cast<int>(camera.extent.x);
        offsetScreenPosition.y += static_cast<int>(camera.extent.y);

        WorldObjectID hitObject{
            worldObjectLocator.getObjectUnderPoint(offsetScreenPosition)};


    }
}

} // End namespace Client
} // End namespace AM
