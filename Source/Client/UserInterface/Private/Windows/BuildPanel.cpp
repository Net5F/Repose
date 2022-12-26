#include "BuildPanel.h"
#include "MainScreen.h"
#include "AssetCache.h"
#include "SpriteData.h"
#include "BuildOverlay.h"
#include "MainThumbnail.h"
#include "SharedConfig.h"
#include "EmptySpriteID.h"
#include "Paths.h"
#include "Ignore.h"
#include "AMAssert.h"
#include <memory>

namespace AM
{
namespace Client
{
BuildPanel::BuildPanel(AssetCache& inAssetCache, SpriteData& inSpriteData,
                       BuildOverlay& inBuildOverlay)
: AUI::Window{{0, 761, 1920, 319}, "BuildPanel"}
, assetCache{inAssetCache}
, spriteData{inSpriteData}
, buildOverlay{inBuildOverlay}
, tileLayerIndex{1}
, backgroundImage{{0, 0, 1920, 319}, "BuildPanelBackground"}
, tileContainer{{366 - 2, 91, 1188, 220}, "TileContainer"}
, layerLabel{{1630, 97, 138, 36}, "LayerLabel"}
, layerDownButton{inAssetCache, {1630, 139, 66, 31}, "<", "LayerDownButton"}
, layerUpButton{inAssetCache, {1704, 139, 66, 31}, ">", "LayerUpButton"}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(tileContainer);
    children.push_back(layerLabel);
    children.push_back(layerDownButton);
    children.push_back(layerUpButton);

    /* Background image */
    backgroundImage.addResolution(
        {1280, 720},
        inAssetCache.loadTexture(Paths::TEXTURE_DIR
                                 + "BuildPanel/Background_1280.png"));
    backgroundImage.addResolution(
        {1600, 900},
        inAssetCache.loadTexture(Paths::TEXTURE_DIR
                                 + "BuildPanel/Background_1600.png"));
    backgroundImage.addResolution(
        {1920, 1080},
        inAssetCache.loadTexture(Paths::TEXTURE_DIR
                                 + "BuildPanel/Background_1920.png"));

    /* Container */
    tileContainer.setNumColumns(11);
    tileContainer.setCellWidth(108);
    tileContainer.setCellHeight(109 + 1);

    // Add the eraser as the first thumbnail.
    addEraser();

    // TODO: We need some tags on our sprites to tell us which ones can be
    //       used as tiles.
    // Fill the container with the available tiles.
    for (const Sprite& sprite : spriteData.getAllSprites()) {
        // Skip the empty sprite.
        // TODO: Once tags are added, we can remove this check since the
        //       empty sprite will be naturally filtered out.
        if (sprite.numericID == -1) {
            continue;
        }

        addTile(sprite);
    }

    /* Layer label */
    layerLabel.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
    layerLabel.setColor({255, 255, 255, 255});
    layerLabel.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
    layerLabel.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    layerLabel.setText("Layer " + std::to_string(tileLayerIndex));

    /* Layer buttons */
    layerDownButton.setOnPressed([&]() {
        if (tileLayerIndex > 0) {
            // Update our label.
            tileLayerIndex--;
            layerLabel.setText("Layer " + std::to_string(tileLayerIndex));

            // Update BuildOverlay.
            buildOverlay.setSelectedLayer(tileLayerIndex);
        }
    });

    layerUpButton.setOnPressed([&]() {
        if ((tileLayerIndex + 1) < SharedConfig::MAX_TILE_LAYERS) {
            // Update our label.
            tileLayerIndex++;
            layerLabel.setText("Layer " + std::to_string(tileLayerIndex));

            // Update BuildOverlay.
            buildOverlay.setSelectedLayer(tileLayerIndex);
        }
    });
}

void BuildPanel::addEraser()
{
    // Construct the new eraser thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<MainThumbnail>(assetCache, "EraserThumbnail")};
    MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Load the eraser's image.
    thumbnail.thumbnailImage.addResolution(
        {1280, 720},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "BuildPanel/EraserIcon_1280.png"));
    thumbnail.thumbnailImage.addResolution(
        {1600, 900},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "BuildPanel/EraserIcon_1600.png"));
    thumbnail.thumbnailImage.addResolution(
        {1920, 1080},
        assetCache.loadTexture(Paths::TEXTURE_DIR
                               + "BuildPanel/EraserIcon_1920.png"));

    // Add a callback to deactivate all other thumbnails when one is activated.
    thumbnail.setOnSelected([this](AUI::Thumbnail* selectedThumb) {
        // Deactivate all other thumbnails.
        for (auto& widgetPtr : tileContainer) {
            MainThumbnail& otherThumb{static_cast<MainThumbnail&>(*widgetPtr)};
            if (otherThumb.getIsSelected() && (&otherThumb != selectedThumb)) {
                otherThumb.deselect();
            }
        }

        // Tell BuildOverlay that the active tile changed to the empty tile.
        buildOverlay.setSelectedTile(spriteData.get(EMPTY_SPRITE_ID));
    });

    tileContainer.push_back(std::move(thumbnailPtr));
}

void BuildPanel::addTile(const Sprite& sprite)
{
    // Construct the new sprite thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<MainThumbnail>(assetCache, "BuildPanelThumbnail")};
    MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Calc a square texture extent that shows the bottom of the sprite (so we 
    // don't have to squash it).
    SpriteRenderData renderData{spriteData.getRenderData(sprite.numericID)};
    SDL_Rect textureExtent{renderData.textureExtent};
    if (textureExtent.h > textureExtent.w) {
        int diff{textureExtent.h - textureExtent.w};
        textureExtent.h -= diff;
        textureExtent.y += diff;
    }

    // Load the sprite's image.
    thumbnail.thumbnailImage.addResolution({1280, 720}, renderData.texture,
                                           textureExtent);

    // Add a callback to deactivate all other thumbnails when one is activated.
    thumbnail.setOnSelected([this, &sprite](AUI::Thumbnail* selectedThumb) {
        // Deactivate all other thumbnails.
        for (auto& widgetPtr : tileContainer) {
            MainThumbnail& otherThumb{static_cast<MainThumbnail&>(*widgetPtr)};
            if (otherThumb.getIsSelected() && (&otherThumb != selectedThumb)) {
                otherThumb.deselect();
            }
        }

        // Tell BuildOverlay that the active tile changed.
        buildOverlay.setSelectedTile(sprite);
    });

    tileContainer.push_back(std::move(thumbnailPtr));
}

} // End namespace Client
} // End namespace AM
