#include "TerrainTool.h"
#include "World.h"
#include "Network.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{

TerrainTool::TerrainTool(World& inWorld, Network& inNetwork)
: BuildTool(inWorld, inNetwork)
, selectedGraphicSet{nullptr}
, selectedGraphicIndex{0}
{
}

void TerrainTool::setSelectedGraphicSet(
    const GraphicSet& inSelectedGraphicSet)
{
    // Note: This cast should be safe, since only terrain graphic sets should 
    //       be clickable while this tool is alive.
    selectedGraphicSet
        = static_cast<const TerrainGraphicSet*>(&inSelectedGraphicSet);

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

void TerrainTool::onMouseDown(AUI::MouseButtonType buttonType,
                                    const SDL_Point&)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active, the user left clicked, and we have a selected
    // graphic.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedGraphicSet != nullptr)) {
        // Tell the sim to add the layer.
        network.serializeAndSend(TileAddLayer{
            mouseTilePosition, TileLayer::Type::Terrain,
            selectedGraphicSet->numericID,
            static_cast<Uint8>(validGraphicIndices[selectedGraphicIndex])});
    }
}

void TerrainTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void TerrainTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active, do nothing.
    if (!isActive) {
        return;
    }

    // Select the next graphic within the set, accounting for negative values.
    selectedGraphicIndex
        = (selectedGraphicIndex + amountScrolled + validGraphicIndices.size())
          % validGraphicIndices.size();

    // Set the newly selected graphic as a phantom at the current location.
    const GraphicRef& graphic{
        selectedGraphicSet
            ->graphics[validGraphicIndices[selectedGraphicIndex]]};
    phantomSprites.clear();
    phantomSprites.emplace_back(
        mouseTilePosition, TileLayer::Type::Terrain, Wall::Type::None,
        Position{}, &(graphic.getFirstSprite()));
}

void TerrainTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // If this tool is active and we have a selected sprite.
    if (isActive && (selectedGraphicSet != nullptr)) {
        // Set the selected graphic as a phantom at the new location.
        const GraphicRef& graphic{
            selectedGraphicSet
                ->graphics[validGraphicIndices[selectedGraphicIndex]]};
        phantomSprites.emplace_back(
            mouseTilePosition, TileLayer::Type::Terrain, Wall::Type::None,
            Position{}, &(graphic.getFirstSprite()));
    }
}

} // End namespace Client
} // End namespace AM
