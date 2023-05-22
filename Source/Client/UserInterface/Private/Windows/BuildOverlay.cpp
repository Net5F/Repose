#include "BuildOverlay.h"
#include "SpriteData.h"
#include "Sprite.h"
#include "Paths.h"
#include "Transforms.h"
#include "ClientTransforms.h"
#include "WorldSinks.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client
{
BuildOverlay::BuildOverlay(SpriteData& inSpriteData, WorldSinks& inWorldSinks,
                           EventDispatcher& inUiEventDispatcher)
: AUI::Window({0, 0, 1920, 744}, "BuildOverlay")
, spriteData{inSpriteData}
, uiEventDispatcher{inUiEventDispatcher}
, selectedTile{nullptr}
, tileLayerIndex{1}
, showTile{false}
, camera{}
, mapTileExtent{}
, mouseTilePosition{}
{
    // We need to know when the map size changes so we can bound the cursor
    // appropriately.
    inWorldSinks.tileMapExtentChanged
        .connect<&BuildOverlay::onTileMapExtentChanged>(*this);
}

void BuildOverlay::setSelectedTile(const Sprite& inSelectedTile)
{
    selectedTile = &inSelectedTile;
}

void BuildOverlay::setSelectedLayer(unsigned int inTileLayerIndex)
{
    tileLayerIndex = inTileLayerIndex;
}

void BuildOverlay::setCamera(const Camera& inCamera)
{
    camera = inCamera;
}

void BuildOverlay::render()
{
    if (showTile && (selectedTile != nullptr)) {
        SpriteRenderData renderData{
            spriteData.getRenderData(selectedTile->numericID)};
        SDL_Rect screenExtent{ClientTransforms::tileToScreenExtent(
            mouseTilePosition, renderData, camera)};

        // Set the texture's alpha to make the tile semi-transparent.
        SDL_SetTextureAlphaMod(renderData.texture.get(), 150);

        // Draw the semi-transparent tile.
        SDL_RenderCopy(AUI::Core::getRenderer(), renderData.texture.get(),
                       &(renderData.textureExtent), &screenExtent);

        // Set the texture's alpha back.
        SDL_SetTextureAlphaMod(renderData.texture.get(), 255);
    }
}

AUI::EventResult BuildOverlay::onMouseDown(AUI::MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);

    // If we don't have a selected tile or the mouse is outside of the world
    // bounds, ignore this event.
    if ((selectedTile == nullptr)
        || !(mapTileExtent.containsPosition(mouseTilePosition))) {
        return AUI::EventResult{.wasHandled{false}};
    }

    // Only respond to the left mouse button.
    if (buttonType == AUI::MouseButtonType::Left) {
        // Send a request to update the clicked tile to the selected sprite.
        //uiEventDispatcher.emplace<TileUpdateRequest>(
        //    mouseTilePosition.x, mouseTilePosition.y, tileLayerIndex,
        //    selectedTile->numericID);
    }

    return AUI::EventResult{.wasHandled{false}};
}

AUI::EventResult BuildOverlay::onMouseMove(const SDL_Point& cursorPosition)
{
    // Get the tile coordinate that the mouse is hovering over.
    ScreenPoint screenPoint{static_cast<float>(cursorPosition.x),
                            static_cast<float>(cursorPosition.y)};
    TilePosition newTilePosition{Transforms::screenToTile(screenPoint, camera)};

    // If the mouse is outside of the world bounds, ignore this event.
    if (!(mapTileExtent.containsPosition(newTilePosition))) {
        return AUI::EventResult{.wasHandled{false}};
    }
    else {
        // The mouse is in bounds, save the new tile position.
        mouseTilePosition = newTilePosition;
        showTile = true;
        return AUI::EventResult{.wasHandled{true}};
    }
}

void BuildOverlay::onMouseLeave()
{
    showTile = false;
}

void BuildOverlay::onTileMapExtentChanged(TileExtent inTileExtent)
{
    mapTileExtent = inTileExtent;
}

} // End namespace Client
} // End namespace AM
