#include "BuildPanel.h"
#include "MainScreen.h"
#include "SpriteData.h"
#include "BuildOverlay.h"
#include "MainThumbnail.h"
#include "SharedConfig.h"
#include "EmptySpriteID.h"
#include "Paths.h"
#include "Ignore.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{
BuildPanel::BuildPanel(SpriteData& inSpriteData, BuildOverlay& inBuildOverlay)
: AUI::Window{{0, 761, 1920, 319}, "BuildPanel"}
, spriteData{inSpriteData}
, buildOverlay{inBuildOverlay}
, selectedThumbnail{nullptr}
, backgroundImage{{0, 0, 1920, 319}, "BuildPanelBackground"}
, tileSpriteSetContainers{AUI::VerticalGridContainer{{366 - 2, 91, 1188, 220},
                                                     "FloorContainer"},
                          AUI::VerticalGridContainer{{366 - 2, 91, 1188, 220},
                                                     "FloorCoveringContainer"},
                          AUI::VerticalGridContainer{{366 - 2, 91, 1188, 220},
                                                     "WallContainer"},
                          AUI::VerticalGridContainer{{366 - 2, 91, 1188, 220},
                                                     "ObjectContainer"}}
, toolsLabel{{152, 92, 138, 36}, "ToolsLabel"}
, buildToolButtons{MainButton{{164, 132, 114, 28}, "Floor", "FloorToolButton"},
                   MainButton{{164, 168, 114, 28}, "Floor Cover", "FloorCoveringToolButton"},
                   MainButton{{164, 204, 114, 28}, "Wall", "WallToolButton"},
                   MainButton{{164, 240, 114, 28}, "Object", "ObjectToolButton"},
                   MainButton{{164, 276, 114, 28}, "Remove", "RemoveToolButton"}}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(tileSpriteSetContainers[TileLayer::Type::Floor]);
    children.push_back(tileSpriteSetContainers[TileLayer::Type::FloorCovering]);
    children.push_back(tileSpriteSetContainers[TileLayer::Type::Wall]);
    children.push_back(tileSpriteSetContainers[TileLayer::Type::Object]);
    children.push_back(toolsLabel);
    children.push_back(buildToolButtons[BuildTool::Type::Floor]);
    children.push_back(buildToolButtons[BuildTool::Type::FloorCovering]);
    children.push_back(buildToolButtons[BuildTool::Type::Wall]);
    children.push_back(buildToolButtons[BuildTool::Type::Object]);
    children.push_back(buildToolButtons[BuildTool::Type::Remove]);

    /* Background image */
    backgroundImage.setMultiResImage(
        {{{1280, 720}, (Paths::TEXTURE_DIR + "BuildPanel/Background_1280.png")},
         {{1600, 900}, (Paths::TEXTURE_DIR + "BuildPanel/Background_1600.png")},
         {{1920, 1080},
          (Paths::TEXTURE_DIR + "BuildPanel/Background_1920.png")}});

    /* Container */
    for (AUI::VerticalGridContainer& container : tileSpriteSetContainers) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
        container.setIsVisible(false);
    }

    /* Tools label */
    toolsLabel.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
    toolsLabel.setColor({255, 255, 255, 255});
    toolsLabel.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
    toolsLabel.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    toolsLabel.setText("Tools");

    /* Build tool buttons. */
    buildToolButtons[BuildTool::Type::Floor].setOnPressed([this]() {
        setBuildTool(BuildTool::Type::Floor);
    });
    buildToolButtons[BuildTool::Type::FloorCovering].setOnPressed([this]() {
        setBuildTool(BuildTool::Type::FloorCovering);
    });
    buildToolButtons[BuildTool::Type::Wall].setOnPressed([this]() {
        setBuildTool(BuildTool::Type::Wall);
    });
    buildToolButtons[BuildTool::Type::Object].setOnPressed([this]() {
        setBuildTool(BuildTool::Type::Object);
    });
    // TODO: Add remove tool

    // Fill the containers with the available sprite sets.
    for (const FloorSpriteSet& spriteSet : spriteData.getAllFloorSpriteSets()) {
        addSpriteSet(TileLayer::Type::Floor, spriteSet, spriteSet.sprite);
    }
    for (const FloorCoveringSpriteSet& spriteSet :
         spriteData.getAllFloorCoveringSpriteSets()) {
        addSpriteSet(TileLayer::Type::FloorCovering, spriteSet,
                     *getFirstSprite(spriteSet));
    }
    for (const WallSpriteSet& spriteSet : spriteData.getAllWallSpriteSets()) {
        addSpriteSet(TileLayer::Type::Wall, spriteSet, spriteSet.sprites[0]);
    }
    for (const ObjectSpriteSet& spriteSet :
         spriteData.getAllObjectSpriteSets()) {
        addSpriteSet(TileLayer::Type::Object, spriteSet,
                     *getFirstSprite(spriteSet));
    }
}

void BuildPanel::addSpriteSet(TileLayer::Type type, const SpriteSet& spriteSet,
    const Sprite& sprite)
{
    // Construct the new sprite thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<MainThumbnail>("BuildPanelThumbnail")};
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
    thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                            textureExtent);

    // Add a callback to deactivate all other thumbnails when one is activated.
    thumbnail.setOnSelected([this, &spriteSet](AUI::Thumbnail* selectedThumb) {
        // If there's an old selection, deselect it.
        if (selectedThumbnail != nullptr) {
            selectedThumbnail->deselect();
        }

        // Set this thumbnail as the new selection.
        selectedThumbnail = selectedThumb;

        // Tell the overlay that the selected sprite changed.
        buildOverlay.setSelectedSpriteSet(spriteSet);
    });

    tileSpriteSetContainers[type].push_back(std::move(thumbnailPtr));
}

void BuildPanel::setBuildTool(BuildTool::Type toolType)
{
    // When we switch build tools, we deselect any selected thumbnails.
    if (selectedThumbnail != nullptr) {
        selectedThumbnail->deselect();
        selectedThumbnail = nullptr;
    }

    // Set the overlay's build tool.
    buildOverlay.setBuildTool(toolType);

    // Make all the containers invisible, then show the correct container.
    for (AUI::VerticalGridContainer& container : tileSpriteSetContainers) {
        container.setIsVisible(false);
    }
    tileSpriteSetContainers[toolType].setIsVisible(true);
}

} // End namespace Client
} // End namespace AM
