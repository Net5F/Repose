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
, selectedHeightIndex{0}
, selectedStartHeight{0}
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
    validHeights.clear();
    for (std::size_t i = 0; i < selectedGraphicSet->graphics.size(); ++i) {
        if (selectedGraphicSet->graphics[i].getGraphicID() != NULL_GRAPHIC_ID) {
            validHeights.emplace_back(i);
        }
    }
    AM_ASSERT(validHeights.size() > 0, "Set didn't contain any graphics.");

    // Select the first height within validHeight.
    selectedHeightIndex = 0;
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
        Terrain::Height height{
            static_cast<Terrain::Height>(validHeights[selectedHeightIndex])};
        network.serializeAndSend(TileAddLayer{
            mouseTilePosition, TileOffset{}, TileLayer::Type::Terrain,
            selectedGraphicSet->numericID,
            Terrain::toValue(height, selectedStartHeight)});
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

    // If ctrl is held, update selectedStartHeight.
    if (SDL_GetModState() & KMOD_CTRL) {
        int newStartHeight{
            static_cast<int>(selectedStartHeight + amountScrolled)};
        newStartHeight = std::clamp(
            newStartHeight, 0, static_cast<int>(Terrain::Height::TwoThird));

        // If the new height would push us too high, return early.
        if ((validHeights[selectedHeightIndex] + newStartHeight)
            >= static_cast<int>(Terrain::MAX_COMBINED_HEIGHT)) {
            return;
        }
        selectedStartHeight = static_cast<Terrain::Height>(newStartHeight);
    }
    else {
        // Ctrl isn't held. Select the next graphic within the set.
        int newSelectedHeightIndex{
            static_cast<int>(selectedHeightIndex + amountScrolled)};
        newSelectedHeightIndex
            = std::clamp(newSelectedHeightIndex, 0,
                         static_cast<int>((validHeights.size() - 1)));

        // If the new height would push us too high, return early.
        if ((validHeights[newSelectedHeightIndex] + selectedStartHeight)
            >= static_cast<int>(Terrain::MAX_COMBINED_HEIGHT)) {
            return;
        }
        selectedHeightIndex = newSelectedHeightIndex;
    }

    // Set the newly selected graphic as a phantom at the current location.
    Terrain::Value value{Terrain::toValue(
        static_cast<Terrain::Height>(validHeights[selectedHeightIndex]),
        selectedStartHeight)};
    phantomSprites.clear();
    phantomSprites.emplace_back(mouseTilePosition, TileOffset{},
                                TileLayer::Type::Terrain, Wall::Type::None,
                                Position{}, selectedGraphicSet,
                                static_cast<Uint8>(value));
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
        Terrain::Value value{Terrain::toValue(
            static_cast<Terrain::Height>(validHeights[selectedHeightIndex]),
            selectedStartHeight)};
        phantomSprites.emplace_back(mouseTilePosition, TileOffset{},
                                    TileLayer::Type::Terrain, Wall::Type::None,
                                    Position{}, selectedGraphicSet,
                                    static_cast<Uint8>(value));
    }
}

} // End namespace Client
} // End namespace AM
