#include "StaticObjectTool.h"
#include "TileAddLayer.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AMAssert.h"

namespace AM
{
namespace Client 
{

StaticObjectTool::StaticObjectTool(const World& inWorld, EventDispatcher& inUiEventDispatcher)
: BuildTool(inWorld, inUiEventDispatcher)
, selectedSpriteSet{nullptr}
, selectedSpriteIndex{0}
{
}

void StaticObjectTool::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    // Note: This cast should be safe, since only object sprite sets should 
    //       be clickable while this tool is alive.
    selectedSpriteSet
        = static_cast<const ObjectSpriteSet*>(&inSelectedSpriteSet);

    // Iterate the set and track which indices contain a sprite.
    validSpriteIndices.clear();
    for (std::size_t i = 0; i < selectedSpriteSet->sprites.size(); ++i) {
        if (selectedSpriteSet->sprites[i] != nullptr) {
            validSpriteIndices.emplace_back(i);
        }
    }
    AM_ASSERT(validSpriteIndices.size() > 0, "Set didn't contain any sprites.");

    // Select the first sprite within validSpriteIndices.
    selectedSpriteIndex = 0;
}

void StaticObjectTool::onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().
    ignore(cursorPosition);

    // If this tool is active, the user left clicked, and we have a selected 
    // sprite.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)
        && (selectedSpriteSet != nullptr)) {
        // Tell the sim to add the layer.
        uiEventDispatcher.emplace<TileAddLayer>(
            mouseTilePosition.x, mouseTilePosition.y,
            TileLayer::Type::Object, selectedSpriteSet->numericID,
            static_cast<Uint8>(validSpriteIndices[selectedSpriteIndex]));
    }
}

void StaticObjectTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void StaticObjectTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active, do nothing.
    if (!isActive) {
        return;
    }

    // Select the next sprite within the set, accounting for negative values.
    selectedSpriteIndex
        = (selectedSpriteIndex + amountScrolled + validSpriteIndices.size())
          % validSpriteIndices.size();

    // Set the newly selected sprite as a phantom at the current location.
    phantomSprites.clear();
    phantomSprites.emplace_back(
        mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Object,
        Wall::Type::None, Position{},
        selectedSpriteSet->sprites[validSpriteIndices[selectedSpriteIndex]]);
}

void StaticObjectTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouseTilePosition and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms.
    phantomSprites.clear();

    // If this tool is active and we have a selected sprite.
    if (isActive && (selectedSpriteSet != nullptr)) {
        // Set the selected sprite as a phantom at the new location.
        phantomSprites.emplace_back(
            mouseTilePosition.x, mouseTilePosition.y, TileLayer::Type::Object,
            Wall::Type::None, Position{},
            selectedSpriteSet
                ->sprites[validSpriteIndices[selectedSpriteIndex]]);
    }
}

} // End namespace Client
} // End namespace AM
