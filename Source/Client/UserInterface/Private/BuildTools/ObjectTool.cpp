#include "ObjectTool.h"
#include "World.h"
#include "Network.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{

ObjectTool::ObjectTool(World& inWorld, Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, selectedGraphicSet{nullptr}
, selectedGraphicIndex{0}
, selectedTileOffset{}
, adjustingXYOffset{false}
, xyOffsetAdjustmentOrigin{}
{
}

void ObjectTool::setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet)
{
    // Note: This cast should be safe, since only object graphic sets should
    //       be clickable while this tool is alive.
    selectedGraphicSet
        = static_cast<const ObjectGraphicSet*>(&inSelectedGraphicSet);

    // Iterate the set and track which indices contain a graphic.
    validGraphicIndices.clear();
    for (std::size_t i = 0; i < selectedGraphicSet->graphics.size(); ++i) {
        if (selectedGraphicSet->graphics[i].getGraphicID() != NULL_GRAPHIC_ID) {
            validGraphicIndices.emplace_back(i);
        }
    }
    AM_ASSERT(validGraphicIndices.size() > 0, "Set didn't contain any graphics.");

    // Select the first graphic within validGraphicIndices.
    selectedGraphicIndex = 0;
}

void ObjectTool::onMouseDown(AUI::MouseButtonType buttonType, const SDL_Point&)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active, the user left clicked, and we have a selected
    // graphic.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedGraphicSet != nullptr)) {
        // Tell the sim to add the layer.
        network.serializeAndSend(TileAddLayer{
            mouseTilePosition, selectedTileOffset, TileLayer::Type::Object,
            selectedGraphicSet->numericID,
            static_cast<Uint8>(validGraphicIndices[selectedGraphicIndex])});
    }
}

void ObjectTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void ObjectTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active or we don't have a selection, do nothing.
    if (!isActive || !selectedGraphicSet) {
        return;
    }

    // If ctrl is held, update selectedTileOffset's Z value.
    if (SDL_GetModState() & KMOD_CTRL) {
        int newOffset{
            std::clamp(selectedTileOffset.z + amountScrolled, 0,
                       static_cast<int>(SharedConfig::TILE_WORLD_HEIGHT))};
        selectedTileOffset.z = static_cast<Uint8>(newOffset);
    }
    else {
        // Ctrl isn't held. Select the next graphic within the set, accounting 
        // for negative values.
        selectedGraphicIndex = (selectedGraphicIndex + amountScrolled
                                + validGraphicIndices.size())
                               % validGraphicIndices.size();
    }

    // Set the newly selected graphic as a phantom at the current location.
    phantomSprites.clear();
    phantomSprites.emplace_back(
        mouseTilePosition, selectedTileOffset, TileLayer::Type::Object,
        Wall::Type::None, Position{}, selectedGraphicSet,
        static_cast<Uint8>(validGraphicIndices[selectedGraphicIndex]));
}

void ObjectTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    TilePosition oldMouseTilePosition{mouseTilePosition};
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // If this tool isn't active or we don't have a selection, do nothing.
    if (!isActive || !selectedGraphicSet) {
        return;
    }

    // If ctrl is held, update our x/y offset.
    if (SDL_GetModState() & KMOD_CTRL) {
        // If we're starting an adjustment, save the current mouse position.
        if (!adjustingXYOffset) {
            xyOffsetAdjustmentOrigin = mouseWorldPoint;
            adjustingXYOffset = true;
        }
        else {
            // We're continuing an adjustment. Keep the mouse in the same tile.
            mouseTilePosition = oldMouseTilePosition;
        }

        // Calc the new offset, relative to the origin position.
        float newOffsetX{mouseWorldPoint.x - xyOffsetAdjustmentOrigin.x};
        float newOffsetY{mouseWorldPoint.y - xyOffsetAdjustmentOrigin.y};

        // Clamp the offset to the tile bounds.
        selectedTileOffset.x = static_cast<Uint8>(
            std::clamp(newOffsetX, 0.f,
                       static_cast<float>(SharedConfig::TILE_WORLD_WIDTH)));
        selectedTileOffset.y = static_cast<Uint8>(
            std::clamp(newOffsetY, 0.f,
                       static_cast<float>(SharedConfig::TILE_WORLD_WIDTH)));
    }
    else {
        // If we just stopped adjusting, reset our adjustment state.
        if (adjustingXYOffset) {
            adjustingXYOffset = false;
            selectedTileOffset = {};
        }

        // If our tile position changed, default our Z offset to line up with 
        // the new tile.
        if (mouseTilePosition != oldMouseTilePosition) {
            if (const Tile* tile{world.tileMap.cgetTile(mouseTilePosition)}) {
                if (const TileLayer*
                     terrain{tile->findLayer(TileLayer::Type::Terrain)}) {
                    Terrain::Height totalHeight{
                        Terrain::getTotalHeight(terrain->graphicValue)};
                    selectedTileOffset.z = static_cast<Uint8>(
                        Terrain::getHeightWorldValue(totalHeight));
                }
            }
        }
    }

    // Set the selected sprite as a phantom at the new location.
    phantomSprites.emplace_back(
        mouseTilePosition, selectedTileOffset, TileLayer::Type::Object,
        Wall::Type::None, Position{}, selectedGraphicSet,
        static_cast<Uint8>(validGraphicIndices[selectedGraphicIndex]));
}

} // End namespace Client
} // End namespace AM
