#include "BuildPanel.h"
#include "MainScreen.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "BuildOverlay.h"
#include "MainThumbnail.h"
#include "EntityTool.h"
#include "EntityTemplatesRequest.h"
#include "SharedConfig.h"
#include "EmptySpriteID.h"
#include "Paths.h"
#include "Ignore.h"
#include "AMAssert.h"

namespace AM
{
namespace Client
{
BuildPanel::BuildPanel(World& inWorld, Network& inNetwork, SpriteData& inSpriteData,
                       BuildOverlay& inBuildOverlay)
: AUI::Window{{0, 761, 1920, 319}, "BuildPanel"}
, network{inNetwork}
, spriteData{inSpriteData}
, buildOverlay{inBuildOverlay}
, selectedThumbnail{nullptr}
, backgroundImage{{0, 0, 1920, 319}, "BuildPanelBackground"}
, floorContainer{{366 - 2, 91, 1188, 220}, "FloorContainer"}
, floorCoveringContainer{{366 - 2, 91, 1188, 220}, "FloorCoveringContainer"}
, wallContainer{{366 - 2, 91, 1188, 220}, "WallContainer"}
, objectContainer{{366 - 2, 91, 1188, 220}, "ObjectContainer"}
, entityPanelContent{inWorld, network, spriteData, *this,
                       {366 - 2, 91, 1188, 220}, "EntityPanelContent"}
, removeHintText{{679, 171, 562, 36}, "RemoveHintText"}
, tileLayersLabel{{152, 92, 138, 36}, "TileLayersLabel"}
, otherLabel{{1630, 92, 138, 36}, "OtherLabel"}
, buildToolButtons{
      MainButton{{164, 132, 114, 32}, "Floor", "FloorToolButton"},
      MainButton{{164, 168, 114, 32}, "Floor Cover", "FloorCoveringToolButton"},
      MainButton{{164, 204, 114, 32}, "Wall", "WallToolButton"},
      MainButton{{164, 240, 114, 32}, "Object", "ObjectToolButton"},
      MainButton{{1642, 132, 114, 32}, "Entity", "EntityToolButton"},
      MainButton{{1642, 168, 114, 32}, "Remove", "RemoveToolButton"}}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(floorContainer);
    children.push_back(floorCoveringContainer);
    children.push_back(wallContainer);
    children.push_back(objectContainer);
    children.push_back(entityPanelContent);
    children.push_back(removeHintText);
    children.push_back(tileLayersLabel);
    children.push_back(otherLabel);
    children.push_back(buildToolButtons[BuildTool::Type::Floor]);
    children.push_back(
        buildToolButtons[BuildTool::Type::FloorCovering]);
    children.push_back(buildToolButtons[BuildTool::Type::Wall]);
    children.push_back(buildToolButtons[BuildTool::Type::Object]);
    children.push_back(buildToolButtons[BuildTool::Type::Entity]);
    children.push_back(buildToolButtons[BuildTool::Type::Remove]);

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
    buildToolButtons[BuildTool::Type::FloorCovering].text.setFont(
        (Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 18);

    buildToolButtons[BuildTool::Type::Floor].setOnPressed(
        [this]() { setBuildTool(BuildTool::Type::Floor); });
    buildToolButtons[BuildTool::Type::FloorCovering].setOnPressed(
        [this]() {
            setBuildTool(BuildTool::Type::FloorCovering);
        });
    buildToolButtons[BuildTool::Type::Wall].setOnPressed(
        [this]() { setBuildTool(BuildTool::Type::Wall); });
    buildToolButtons[BuildTool::Type::Object].setOnPressed(
        [this]() { setBuildTool(BuildTool::Type::Object); });
    buildToolButtons[BuildTool::Type::Entity].setOnPressed(
        [this]() { setBuildTool(BuildTool::Type::Entity); });
    buildToolButtons[BuildTool::Type::Remove].setOnPressed(
        [this]() { setBuildTool(BuildTool::Type::Remove); });

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
        std::make_unique<MainThumbnail>("BuildPanelThumbnail")};
    MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
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

    // Note: The sprite editor assures that every floor covering and 
    //       object has at least 1 sprite, so this shouldn't happen.
    LOG_FATAL("Failed to find sprite when expected.");
    return nullptr;
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

    // Make all the content invisible, then show the correct content.
    floorContainer.setIsVisible(false);
    floorCoveringContainer.setIsVisible(false);
    wallContainer.setIsVisible(false);
    objectContainer.setIsVisible(false);
    entityPanelContent.setIsVisible(false);
    removeHintText.setIsVisible(false);

    if (toolType == BuildTool::Type::Floor) {
        floorContainer.setIsVisible(true);
    }
    else if (toolType == BuildTool::Type::FloorCovering) {
        floorCoveringContainer.setIsVisible(true);
    }
    else if (toolType == BuildTool::Type::Wall) {
        wallContainer.setIsVisible(true);
    }
    else if (toolType == BuildTool::Type::Object) {
        objectContainer.setIsVisible(true);
    }
    else if (toolType == BuildTool::Type::Entity) {
        entityPanelContent.setBuildTool(static_cast<EntityTool*>(
            buildOverlay.getCurrentBuildTool()));
        entityPanelContent.setIsVisible(true);

        // Request the latest entity templates from the server.
        network.serializeAndSend<EntityTemplatesRequest>({});
    }
    else if (toolType == BuildTool::Type::Remove) {
        removeHintText.setIsVisible(true);
    }
}

} // End namespace Client
} // End namespace AM
