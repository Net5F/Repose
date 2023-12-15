#include "BuildPanel.h"
#include "MainScreen.h"
#include "Simulation.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "IconData.h"
#include "BuildOverlay.h"
#include "BuildModeThumbnail.h"
#include "EntityTool.h"
#include "SharedConfig.h"
#include "Paths.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{
BuildPanel::BuildPanel(Simulation& inSimulation, Network& inNetwork,
                       SpriteData& inSpriteData, IconData& inIconData,
                       BuildOverlay& inBuildOverlay)
: AUI::Window{{0, 761, 1920, 319}, "BuildPanel"}
, network{inNetwork}
, spriteData{inSpriteData}
, buildOverlay{inBuildOverlay}
, selectedThumbnail{nullptr}
, backgroundImage{{0, 0, 1920, 319}, "BuildPanelBackground"}
, floorContainer{{366, 91, 1188, 220}, "FloorContainer"}
, floorCoveringContainer{{366 - 2, 91, 1188, 220}, "FloorCoveringContainer"}
, wallContainer{{366, 91, 1188, 220}, "WallContainer"}
, objectContainer{{366, 91, 1188, 220}, "ObjectContainer"}
, entityPanelContent{inSimulation.getWorld(),
                     network,
                     spriteData,
                     *this,
                     {366, 91, 1188, 220},
                     "EntityPanelContent"}
, removeHintText{{679, 171, 562, 36}, "RemoveHintText"}
, itemPanelContent{inSimulation,
                   network,
                   inIconData,
                   {366, 91, 1188, 220},
                   "ItemPanelContent"}
, tileLayersLabel{{152, 92, 138, 36}, "TileLayersLabel"}
, otherLabel{{1630, 92, 138, 36}, "OtherLabel"}
, buildModeButtons{
      MainButton{{164, 132, 114, 32}, "Floor", "FloorToolButton"},
      MainButton{{164, 168, 114, 32}, "Floor Cover", "FloorCoveringToolButton"},
      MainButton{{164, 204, 114, 32}, "Wall", "WallToolButton"},
      MainButton{{164, 240, 114, 32}, "Object", "ObjectToolButton"},
      MainButton{{1642, 132, 114, 32}, "Entity", "EntityToolButton"},
      MainButton{{1642, 168, 114, 32}, "Remove", "RemoveToolButton"},
      MainButton{{1642, 204, 114, 32}, "Item", "ItemToolButton"}}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(floorContainer);
    children.push_back(floorCoveringContainer);
    children.push_back(wallContainer);
    children.push_back(objectContainer);
    children.push_back(entityPanelContent);
    children.push_back(removeHintText);
    children.push_back(itemPanelContent);
    children.push_back(tileLayersLabel);
    children.push_back(otherLabel);
    children.push_back(buildModeButtons[BuildMode::Type::Floor]);
    children.push_back(
        buildModeButtons[BuildMode::Type::FloorCovering]);
    children.push_back(buildModeButtons[BuildMode::Type::Wall]);
    children.push_back(buildModeButtons[BuildMode::Type::Object]);
    children.push_back(buildModeButtons[BuildMode::Type::Entity]);
    children.push_back(buildModeButtons[BuildMode::Type::Remove]);
    children.push_back(buildModeButtons[BuildMode::Type::Item]);

    /* Background image */
    backgroundImage.setMultiResImage(
        {{{1280, 720}, (Paths::TEXTURE_DIR + "BuildPanel/Background_1280.png")},
         {{1600, 900}, (Paths::TEXTURE_DIR + "BuildPanel/Background_1600.png")},
         {{1920, 1080},
          (Paths::TEXTURE_DIR + "BuildPanel/Background_1920.png")}});

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
        container.setIsVisible(false);
    };
    setContainerStyle(floorContainer);
    setContainerStyle(floorCoveringContainer);
    setContainerStyle(wallContainer);
    setContainerStyle(objectContainer);
    entityPanelContent.setIsVisible(false);
    itemPanelContent.setIsVisible(false);

    /* Labels */
    auto setTextStyle = [](AUI::Text& text) {
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
        text.setColor({255, 255, 255, 255});
        text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
        text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    };
    setTextStyle(tileLayersLabel);
    tileLayersLabel.setText("Tile Layers");

    setTextStyle(otherLabel);
    otherLabel.setText("Other");

    setTextStyle(removeHintText);
    removeHintText.setText("Click on a Tile Layer or Entity to remove it.");
    removeHintText.setIsVisible(false);

    /* Build tool buttons. */
    buildModeButtons[BuildMode::Type::FloorCovering].text.setFont(
        (Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 18);

    buildModeButtons[BuildMode::Type::Floor].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Floor); });
    buildModeButtons[BuildMode::Type::FloorCovering].setOnPressed(
        [this]() {
            setBuildMode(BuildMode::Type::FloorCovering);
        });
    buildModeButtons[BuildMode::Type::Wall].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Wall); });
    buildModeButtons[BuildMode::Type::Object].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Object); });
    buildModeButtons[BuildMode::Type::Entity].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Entity); });
    buildModeButtons[BuildMode::Type::Remove].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Remove); });
    // TODO: Segfaulting when pressing this
    buildModeButtons[BuildMode::Type::Item].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Item); });

    // Fill the containers with the available sprite sets.
    for (const FloorSpriteSet& spriteSet : spriteData.getAllFloorSpriteSets()) {
        addTileSpriteSet(TileLayer::Type::Floor, spriteSet, spriteSet.sprite);
    }
    for (const FloorCoveringSpriteSet& spriteSet :
         spriteData.getAllFloorCoveringSpriteSets()) {
        addTileSpriteSet(TileLayer::Type::FloorCovering, spriteSet,
                     *getFirstSprite(spriteSet));
    }
    for (const WallSpriteSet& spriteSet : spriteData.getAllWallSpriteSets()) {
        addTileSpriteSet(TileLayer::Type::Wall, spriteSet, spriteSet.sprites[0]);
    }
    for (const ObjectSpriteSet& spriteSet :
         spriteData.getAllObjectSpriteSets()) {
        addTileSpriteSet(TileLayer::Type::Object, spriteSet,
                     *getFirstSprite(spriteSet));
    }
}

void BuildPanel::setSelectedThumbnail(AUI::Thumbnail& newSelectedThumbnail)
{
    // If there's an old selection, deselect it.
    clearSelectedThumbnail();

    // Set this thumbnail as the new selection.
    selectedThumbnail = &newSelectedThumbnail;
}

void BuildPanel::clearSelectedThumbnail()
{
    // If there's an old selection, deselect it.
    if (selectedThumbnail != nullptr) {
        selectedThumbnail->deselect();
        selectedThumbnail = nullptr;
    }
}

void BuildPanel::addTileSpriteSet(TileLayer::Type type, const SpriteSet& spriteSet,
    const Sprite& sprite)
{
    // Construct the new sprite thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<BuildModeThumbnail>("BuildPanelThumbnail")};
    BuildModeThumbnail& thumbnail{static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Calc a square texture extent that shows the bottom of the sprite (so we
    // don't have to squash it).
    const SpriteRenderData& renderData{
        spriteData.getRenderData(sprite.numericID)};
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
        // Set this thumbnail as the new selection.
        setSelectedThumbnail(*selectedThumb);

        // Tell the overlay that the selected sprite changed.
        buildOverlay.setSelectedSpriteSet(spriteSet);
    });

    if (type == TileLayer::Type::Floor) {
        floorContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::FloorCovering) {
        floorCoveringContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::Wall) {
        wallContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::Object) {
        objectContainer.push_back(std::move(thumbnailPtr));
    }
}

template<typename T>
requires std::same_as<T, FloorCoveringSpriteSet>
         || std::same_as<T, ObjectSpriteSet>
const Sprite* BuildPanel::getFirstSprite(const T& spriteSet)
{
    for (const Sprite* sprite : spriteSet.sprites) {
        if (sprite != nullptr) {
            return sprite;
        }
    }

    // Note: The resource importer assures that every floor covering and 
    //       object has at least 1 sprite, so this shouldn't happen.
    LOG_FATAL("Failed to find sprite when expected.");
    return nullptr;
}

void BuildPanel::setBuildMode(BuildMode::Type buildModeType)
{
    // When we switch build tools, we deselect any selected thumbnails.
    if (selectedThumbnail != nullptr) {
        selectedThumbnail->deselect();
        selectedThumbnail = nullptr;
    }

    // Set the overlay to the given build mode.
    buildOverlay.setBuildMode(buildModeType);

    // Make all the content invisible, then show the correct content.
    floorContainer.setIsVisible(false);
    floorCoveringContainer.setIsVisible(false);
    wallContainer.setIsVisible(false);
    objectContainer.setIsVisible(false);
    entityPanelContent.setIsVisible(false);
    removeHintText.setIsVisible(false);
    itemPanelContent.setIsVisible(false);

    if (buildModeType == BuildMode::Type::Floor) {
        floorContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::FloorCovering) {
        floorCoveringContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Wall) {
        wallContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Object) {
        objectContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Entity) {
        entityPanelContent.setBuildTool(static_cast<EntityTool*>(
            buildOverlay.getCurrentBuildTool()));
        entityPanelContent.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Remove) {
        removeHintText.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Item) {
        itemPanelContent.reset();
        itemPanelContent.setIsVisible(true);
    }
}

} // End namespace Client
} // End namespace AM
