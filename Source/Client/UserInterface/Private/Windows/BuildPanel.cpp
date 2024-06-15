#include "BuildPanel.h"
#include "MainScreen.h"
#include "Simulation.h"
#include "World.h"
#include "Network.h"
#include "GraphicData.h"
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
                       GraphicData& inGraphicData, IconData& inIconData,
                       BuildOverlay& inBuildOverlay)
: AUI::Window{{0, 761, 1920, 319}, "BuildPanel"}
, network{inNetwork}
, graphicData{inGraphicData}
, buildOverlay{inBuildOverlay}
, selectedThumbnail{nullptr}
, backgroundImage{{0, 0, 1920, 319}, "BuildPanelBackground"}
, terrainContainer{{366, 91, 1188, 220}, "TerrainContainer"}
, floorContainer{{366 - 2, 91, 1188, 220}, "FloorContainer"}
, wallContainer{{366, 91, 1188, 220}, "WallContainer"}
, objectContainer{{366, 91, 1188, 220}, "ObjectContainer"}
, entityPanelContent{inSimulation.getWorld(),
                     network,
                     graphicData,
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
      MainButton{{164, 132, 114, 32}, "Terrain", "TerrainToolButton"},
      MainButton{{164, 168, 114, 32}, "Floor", "FloorToolButton"},
      MainButton{{164, 204, 114, 32}, "Wall", "WallToolButton"},
      MainButton{{164, 240, 114, 32}, "Object", "ObjectToolButton"},
      MainButton{{1642, 132, 114, 32}, "Entity", "EntityToolButton"},
      MainButton{{1642, 168, 114, 32}, "Remove", "RemoveToolButton"},
      MainButton{{1642, 204, 114, 32}, "Item", "ItemToolButton"}}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(terrainContainer);
    children.push_back(floorContainer);
    children.push_back(wallContainer);
    children.push_back(objectContainer);
    children.push_back(entityPanelContent);
    children.push_back(removeHintText);
    children.push_back(itemPanelContent);
    children.push_back(tileLayersLabel);
    children.push_back(otherLabel);
    children.push_back(buildModeButtons[BuildMode::Type::Terrain]);
    children.push_back(buildModeButtons[BuildMode::Type::Floor]);
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
    setContainerStyle(terrainContainer);
    setContainerStyle(floorContainer);
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
    buildModeButtons[BuildMode::Type::Terrain].text.setFont(
        (Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 18);

    buildModeButtons[BuildMode::Type::Terrain].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Terrain); });
    buildModeButtons[BuildMode::Type::Floor].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Floor); });
    buildModeButtons[BuildMode::Type::Wall].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Wall); });
    buildModeButtons[BuildMode::Type::Object].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Object); });
    buildModeButtons[BuildMode::Type::Entity].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Entity); });
    buildModeButtons[BuildMode::Type::Remove].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Remove); });
    buildModeButtons[BuildMode::Type::Item].setOnPressed(
        [this]() { setBuildMode(BuildMode::Type::Item); });

    // Fill the containers with the available graphic sets.
    for (const TerrainGraphicSet& graphicSet :
         graphicData.getAllTerrainGraphicSets()) {
        if (!(graphicSet.numericID)) {
            continue;
        }
        addTileGraphicSet(TileLayer::Type::Terrain, graphicSet,
                         getFirstSprite(graphicSet));
    }
    for (const FloorGraphicSet& graphicSet : graphicData.getAllFloorGraphicSets()) {
        if (!(graphicSet.numericID)) {
            continue;
        }
        addTileGraphicSet(TileLayer::Type::Floor, graphicSet,
                          getFirstSprite(graphicSet));
    }
    for (const WallGraphicSet& graphicSet : graphicData.getAllWallGraphicSets()) {
        if (!(graphicSet.numericID)) {
            continue;
        }
        addTileGraphicSet(TileLayer::Type::Wall, graphicSet,
                         graphicSet.graphics[0].getFirstSprite());
    }
    for (const ObjectGraphicSet& graphicSet :
         graphicData.getAllObjectGraphicSets()) {
        if (!(graphicSet.numericID)) {
            continue;
        }
        addTileGraphicSet(TileLayer::Type::Object, graphicSet,
                         getFirstSprite(graphicSet));
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

void BuildPanel::addTileGraphicSet(TileLayer::Type type,
                                   const GraphicSet& graphicSet,
                                   const Sprite& sprite)
{
    // Construct the new graphic set thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<BuildModeThumbnail>("BuildPanelThumbnail")};
    BuildModeThumbnail& thumbnail{
        static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Calc a square texture extent that shows the bottom of the sprite (so we
    // don't have to squash it).
    const SpriteRenderData& renderData{
        graphicData.getRenderData(sprite.numericID)};
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
    thumbnail.setOnSelected([this, &graphicSet](AUI::Thumbnail* selectedThumb) {
        // Set this thumbnail as the new selection.
        setSelectedThumbnail(*selectedThumb);

        // Tell the overlay that the selected graphic set changed.
        buildOverlay.setSelectedGraphicSet(graphicSet);
    });

    if (type == TileLayer::Type::Terrain) {
        terrainContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::Floor) {
        floorContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::Wall) {
        wallContainer.push_back(std::move(thumbnailPtr));
    }
    else if (type == TileLayer::Type::Object) {
        objectContainer.push_back(std::move(thumbnailPtr));
    }
}

template<typename T>
    requires std::same_as<T, TerrainGraphicSet>
             || std::same_as<T, FloorGraphicSet>
             || std::same_as<T, ObjectGraphicSet>
const Sprite& BuildPanel::getFirstSprite(const T& graphicSet)
{
    for (const GraphicRef& graphic : graphicSet.graphics) {
        if (graphic.getGraphicID() != NULL_GRAPHIC_ID) {
            return graphic.getFirstSprite();
        }
    }

    return graphicData.getSprite(NULL_SPRITE_ID);
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
    terrainContainer.setIsVisible(false);
    floorContainer.setIsVisible(false);
    wallContainer.setIsVisible(false);
    objectContainer.setIsVisible(false);
    entityPanelContent.setIsVisible(false);
    removeHintText.setIsVisible(false);
    itemPanelContent.setIsVisible(false);

    if (buildModeType == BuildMode::Type::Terrain) {
        terrainContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Floor) {
        floorContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Wall) {
        wallContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Object) {
        objectContainer.setIsVisible(true);
    }
    else if (buildModeType == BuildMode::Type::Entity) {
        entityPanelContent.setBuildTool(
            static_cast<EntityTool*>(buildOverlay.getCurrentBuildTool()));
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
