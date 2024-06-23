#include "RemoveTool.h"
#include "World.h"
#include "Network.h"
#include "WorldObjectLocator.h"
#include "TileRemoveLayer.h"
#include "World.h"
#include "Tile.h"
#include "EntityDeleteRequest.h"
#include "QueuedEvents.h"

namespace AM
{
namespace Client
{

RemoveTool::RemoveTool(World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, worldObjectLocator{inWorldObjectLocator}
, highlightColor{255, 220, 0, 255}
{
}

void RemoveTool::onMouseDown(AUI::MouseButtonType buttonType,
                             const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active and the user left clicked.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
        // Get the first world object under the mouse.
        WorldObjectID objectID{
            worldObjectLocator.getObjectUnderPoint(cursorPosition)};

        // If we hit a removable object, tell the sim to remove it.
        if (TileLayerID* layer{std::get_if<TileLayerID>(&objectID)}) {
            requestRemoveTileLayer(layer->tilePosition, layer->tileOffset,
                                   layer->type, layer->graphicSetID,
                                   layer->graphicValue);
        }
        else if (entt::entity* entity{std::get_if<entt::entity>(&objectID)}) {
            network.serializeAndSend(EntityDeleteRequest{*entity});
        }
        else {
            // Didn't hit a removable object. Tell the sim to remove the terrain.
            network.serializeAndSend(
                TileRemoveLayer{mouseTilePosition, TileOffset{},
                                TileLayer::Type::Terrain, 0, 0});
        }
    }
}

void RemoveTool::onMouseUp(AUI::MouseButtonType, const SDL_Point&)
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
        // Clear any old color mods.
        spriteColorMods.clear();

        // Get the first world object under the mouse.
        WorldObjectID objectID{
            worldObjectLocator.getObjectUnderPoint(cursorPosition)};

        // If we're hovering over an object, add a color mod for it.
        if (!std::holds_alternative<std::monostate>(objectID)) {
            // Ignore NW gap fills (the user has to remove one of the adjoining
            // walls instead).
            TileLayerID* layer{std::get_if<TileLayerID>(&objectID)};
            if (layer && (layer->type == TileLayer::Type::Wall)
                && (layer->graphicValue == Wall::Type::NorthWestGapFill)) {
                return;
            }

            // Note: If the object was deleted but is still in the vector,
            //       this is still safe (the color mod just won't be used).
            spriteColorMods.emplace_back(objectID, highlightColor);
        }
    }
}

void RemoveTool::requestRemoveTileLayer(const TilePosition& tilePosition,
                                        const TileOffset& tileOffset,
                                        TileLayer::Type layerType,
                                        Uint16 graphicSetID, Uint8 graphicIndex)
{
    if (layerType == TileLayer::Type::Wall) {
        Wall::Type wallType{graphicIndex};

        // Ignore NW gap fills (they'll be removed when one of the adjoined
        // walls is removed.
        if (wallType == Wall::Type::NorthWestGapFill) {
            return;
        }

        // If it's a NE gap fill, push a North instead (the tile map handles
        // gap fills).
        if (wallType == Wall::Type::NorthEastGapFill) {
            wallType = Wall::Type::North;
        }

        network.serializeAndSend(TileRemoveLayer{
            tilePosition, tileOffset, layerType, graphicSetID, wallType});
    }
    else {
        network.serializeAndSend(TileRemoveLayer{
            tilePosition, tileOffset, layerType, graphicSetID, graphicIndex});
    }
}

} // End namespace Client
} // End namespace AM
