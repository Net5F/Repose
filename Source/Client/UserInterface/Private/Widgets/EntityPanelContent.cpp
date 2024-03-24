#include "EntityPanelContent.h"
#include "World.h"
#include "Network.h"
#include "GraphicData.h"
#include "BuildPanel.h"
#include "BuildModeThumbnail.h"
#include "EntityTool.h"
#include "Name.h"
#include "Position.h"
#include "Rotation.h"
#include "GraphicState.h"
#include "EntityTemplatesRequest.h"
#include "EntityInitRequest.h"
#include "EntityNameChangeRequest.h"
#include "GraphicStateChangeRequest.h"
#include "EntityInitScriptRequest.h"
#include "AddEntityTemplate.h"
#include "Paths.h"
#include "AMAssert.h"
#include "entt/entity/entity.hpp"
#include <fstream>
#include <sstream>

namespace AM
{
namespace Client
{
EntityPanelContent::EntityPanelContent(World& inWorld, Network& inNetwork,
                                       GraphicData& inGraphicData,
                                       BuildPanel& inBuildPanel,
                                       const SDL_Rect& inScreenExtent,
                                       const std::string& inDebugName)
: AUI::Widget(inScreenExtent, inDebugName)
, world{inWorld}
, network{inNetwork}
, graphicData{inGraphicData}
, buildPanel{inBuildPanel}
, hasRequestedTemplates{false}
, currentView{ViewType::Template}
, entityTool{nullptr}
, editingEntityID{entt::null}
, editingEntityInitScript{""}
, selectedSpriteThumbnail{nullptr}
, entityTemplatesQueue{inNetwork.getEventDispatcher()}
, entityInitScriptQueue{inNetwork.getEventDispatcher()}
// Note: These dimensions are based on the top left that BuildPanel gives us.
, templateContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                    "TemplateContainer"}
, nameLabel{{464, 8, 260, 36}, "NameLabel"}
, nameInput{{464, 53, 260, 42}, "NameInput"}
, changeSpriteButton{{342, 117, 160, 36}, "Change Sprite", "ChangeSpriteButton"}
, editScriptButton{{516, 117, 156, 36}, "Edit Script", "EditScriptButton"}
, saveTemplateButton{{686, 117, 188, 36},
                     "Save as Template",
                     "SaveTemplateButton"}
, graphicSetContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                     "GraphicSetContainer"}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(templateContainer);
    children.push_back(nameLabel);
    children.push_back(nameInput);
    children.push_back(changeSpriteButton);
    children.push_back(editScriptButton);
    children.push_back(saveTemplateButton);
    children.push_back(graphicSetContainer);

    /* Label */
    auto setTextStyle = [](AUI::Text& text) {
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
        text.setColor({255, 255, 255, 255});
        text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
        text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    };
    setTextStyle(nameLabel);
    nameLabel.setText("Entity Name");

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
    };
    setContainerStyle(templateContainer);
    setContainerStyle(graphicSetContainer);

    /* Text input */
    nameInput.normalImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
    nameInput.hoveredImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Hovered.png"), {8, 8, 8, 8});
    nameInput.focusedImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Focused.png"), {8, 8, 8, 8});
    nameInput.setTextFont(((Paths::FONT_DIR + "Cagliostro-Regular.ttf")), 20);
    nameInput.setTextColor({255, 255, 255, 255});
    nameInput.setPadding({0, 14, 0, 14});
    nameInput.setCursorWidth(2);
    nameInput.setCursorColor({255, 255, 255, 255});

    nameInput.setOnTextCommitted([this]() {
        // Send a request to change the entity's name.
        EntityNameChangeRequest nameChangeRequest{editingEntityID,
                                                  Name{nameInput.getText()}};
        network.serializeAndSend(nameChangeRequest);
    });

    /* Buttons */
    changeSpriteButton.setOnPressed(
        [this]() { changeView(ViewType::SpriteSet); });

    editScriptButton.setOnPressed([this]() {
        // Send a re-init request with the updated script.
        const entt::registry& registry{world.registry};
        std::string initScript{""};
        // TEMP - Replace this when we add a text editor UI.
        std::ifstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
        if (scriptFile.is_open()) {
            std::stringstream buffer;
            buffer << scriptFile.rdbuf();
            initScript = buffer.str();
        }
        // TEMP
        network.serializeAndSend(EntityInitRequest{
            editingEntityID, registry.get<Name>(editingEntityID),
            registry.get<Position>(editingEntityID),
            registry.get<GraphicState>(editingEntityID), initScript});
    });

    saveTemplateButton.setOnPressed([this]() {
        network.serializeAndSend(AddEntityTemplate{editingEntityID});
    });

    // Add the thumbnails that we can (we get some later from the server).
    addRefreshButton();
    addDefaultTemplateThumbnail();
    addSpriteSetThumbnails();

    // Hide any non-template-view widgets.
    changeView(ViewType::Template);
}

void EntityPanelContent::setBuildTool(EntityTool* inEntityTool)
{
    entityTool = inEntityTool;

    // New tool, reset the view.
    changeView(ViewType::Template);

    // Register our callbacks.
    if (entityTool != nullptr) {
        entityTool->setOnEntitySelected([this](entt::entity entity) {
            // Save the entity's data.
            editingEntityID = entity;

            // Ask the server for the entity's init script.
            network.serializeAndSend(EntityInitScriptRequest{editingEntityID});

            // Switch to the edit view.
            changeView(ViewType::Edit);
        });

        entityTool->setOnSelectionCleared([this]() {
            buildPanel.clearSelectedThumbnail();
            if (currentView != ViewType::Template) {
                changeView(ViewType::Template);
            }
        });
    }
}

void EntityPanelContent::setIsVisible(bool inIsVisible)
{
    // The first time we're made visible, request the latest entity templates
    // from the server.
    if (!hasRequestedTemplates && inIsVisible) {
        network.serializeAndSend<EntityTemplatesRequest>({});

        hasRequestedTemplates = true;
    }

    Widget::setIsVisible(inIsVisible);
}

void EntityPanelContent::onTick(double)
{
    // Process any waiting messages.
    EntityTemplates entityTemplates{};
    while (entityTemplatesQueue.pop(entityTemplates)) {
        // Clear any existing entity templates (skipping the "refresh" and
        // "default entity" thumbnails).
        if (templateContainer.size() > 2) {
            templateContainer.erase(templateContainer.begin() + 2,
                                    templateContainer.end());
        }

        addTemplateThumbnails(entityTemplates);
    }

    EntityInitScriptResponse initScriptResponse{};
    while (entityInitScriptQueue.pop(initScriptResponse)) {
        // If the received script is for the currently selected entity, save it.
        if (initScriptResponse.entity == editingEntityID) {
            editingEntityInitScript = initScriptResponse.initScript.script;

            // TEMP
            // Write to InitScript.lua
            std::ofstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
            scriptFile << initScriptResponse.initScript.script;
            LOG_INFO("Received entity script. Saved to InitScript.lua");
            // TEMP
        }
    }
}

void EntityPanelContent::setEntityToEdit(entt::entity newEditingEntityID)
{
    editingEntityID = newEditingEntityID;

    // Set the name input's text to match the new entity.
    nameInput.setText(world.registry.get<Name>(editingEntityID).value);
}

void EntityPanelContent::changeView(ViewType newView)
{
    // Hide everything.
    templateContainer.setIsVisible(false);
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    changeSpriteButton.setIsVisible(false);
    editScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);
    graphicSetContainer.setIsVisible(false);

    // Show the new view's widgets.
    if (newView == ViewType::Template) {
        buildPanel.clearSelectedThumbnail();
        templateContainer.setIsVisible(true);
    }
    else if (newView == ViewType::Edit) {
        // Update the name component's data.
        nameInput.setText(world.registry.get<Name>(editingEntityID).value);

        nameLabel.setIsVisible(true);
        nameInput.setIsVisible(true);
        changeSpriteButton.setIsVisible(true);
        editScriptButton.setIsVisible(true);
        saveTemplateButton.setIsVisible(true);
    }
    else if (newView == ViewType::SpriteSet) {
        // TODO: Select the correct thumbnail
        graphicSetContainer.setIsVisible(true);
    }

    currentView = newView;
}

void EntityPanelContent::addRefreshButton()
{
    // Construct the new thumbnail.
    std::unique_ptr<AUI::Widget> buttonPtr{std::make_unique<AUI::Button>(
        SDL_Rect{6, 5, 96, 96}, "RefreshTemplatesButton")};
    AUI::Button& button{static_cast<AUI::Button&>(*buttonPtr)};

    button.text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
    button.text.setText("");

    // Load the "refresh" icon.
    button.normalImage.setMultiResImage(
        {{{1920, 1080},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Normal_1920.png"},
         {{1600, 900},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Normal_1600.png"},
         {{1280, 720},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Normal_1280.png"}});
    button.hoveredImage.setMultiResImage(
        {{{1920, 1080},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Hovered_1920.png"},
         {{1600, 900},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Hovered_1600.png"},
         {{1280, 720},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Hovered_1280.png"}});
    button.pressedImage.setMultiResImage(
        {{{1920, 1080},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Pressed_1920.png"},
         {{1600, 900},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Pressed_1600.png"},
         {{1280, 720},
          Paths::TEXTURE_DIR + "BuildPanel/RefreshIcon_Pressed_1280.png"}});

    // Add the callback.
    button.setOnPressed([this]() {
        // Request the latest entity templates from the server.
        network.serializeAndSend<EntityTemplatesRequest>({});
    });

    templateContainer.push_back(std::move(buttonPtr));
}

void EntityPanelContent::addDefaultTemplateThumbnail()
{
    // Construct the new thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<BuildModeThumbnail>("EntityThumbnail")};
    BuildModeThumbnail& thumbnail{
        static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Get the default graphic.
    // Note: IdleSouth is guaranteed to be present in every entity graphic set.
    const EntityGraphicSet& graphicSet{graphicData.getEntityGraphicSet(
        SharedConfig::DEFAULT_ENTITY_GRAPHIC_SET)};
    const GraphicRef& graphic{
        graphicSet.graphics.at(EntityGraphicType::IdleSouth)};

    // Calc a square texture extent that shows the bottom of the sprite
    // (so we don't have to squash it).
    const SpriteRenderData& renderData{
        graphicData.getRenderData(graphic.getFirstSprite().numericID)};
    SDL_Rect textureExtent{calcSquareTexExtent(renderData)};

    // Load the sprite's image.
    thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                            textureExtent);

    // Add the callback.
    thumbnail.setOnSelected([this](AUI::Thumbnail* selectedThumb) {
        // Set this thumbnail as the new selection.
        buildPanel.setSelectedThumbnail(*selectedThumb);

        // Tell the tool that the selection changed.
        const EntityGraphicSet& graphicSet{graphicData.getEntityGraphicSet(
            SharedConfig::DEFAULT_ENTITY_GRAPHIC_SET)};
        GraphicState graphicState{graphicSet.numericID};
        entityTool->setSelectedTemplate({"Default"}, graphicState);
    });

    templateContainer.push_back(std::move(thumbnailPtr));
}

void EntityPanelContent::addTemplateThumbnails(
    const EntityTemplates& entityTemplates)
{
    // Add thumbnails for all of the given entity templates.
    for (const auto& entityData : entityTemplates.templates) {
        // Construct the new thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<BuildModeThumbnail>("EntityThumbnail")};
        BuildModeThumbnail& thumbnail{
            static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Get the graphic.
        // Note: IdleSouth is guaranteed to be present in every entity set.
        const EntityGraphicSet& graphicSet{graphicData.getEntityGraphicSet(
            entityData.graphicState.graphicSetID)};
        const GraphicRef& graphic{
            graphicSet.graphics.at(EntityGraphicType::IdleSouth)};

        // Calc a square texture extent that shows the bottom of the sprite
        // (so we don't have to squash it).
        const SpriteRenderData& renderData{
            graphicData.getRenderData(graphic.getFirstSprite().numericID)};
        SDL_Rect textureExtent{calcSquareTexExtent(renderData)};

        // Load the sprite's image.
        thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                                textureExtent);

        // Add the callback.
        thumbnail.setOnSelected(
            [this, entityData](AUI::Thumbnail* selectedThumb) {
                // Set this thumbnail as the new selection.
                buildPanel.setSelectedThumbnail(*selectedThumb);

                // Tell the tool that the selection changed.
                entityTool->setSelectedTemplate(entityData.name,
                                                entityData.graphicState);
            });

        templateContainer.push_back(std::move(thumbnailPtr));
    }
}

void EntityPanelContent::addSpriteSetThumbnails()
{
    // Add thumbnails for all entity graphic sets.
    for (const EntityGraphicSet& graphicSet :
         graphicData.getAllEntityGraphicSets()) {
        // Skip the null set.
        if (!(graphicSet.numericID)) {
            continue;
        }

        // Construct the new thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<BuildModeThumbnail>("EntityThumbnail")};
        BuildModeThumbnail& thumbnail{
            static_cast<BuildModeThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Calc a square texture extent that shows the bottom of the graphic
        // (so we don't have to squash it).
        // Note: IdleSouth is guaranteed to be present in every entity set.
        const Sprite& sprite{
            graphicSet.graphics.at(EntityGraphicType::IdleSouth)
                .getFirstSprite()};
        const SpriteRenderData& renderData{
            graphicData.getRenderData(sprite.numericID)};
        SDL_Rect textureExtent{calcSquareTexExtent(renderData)};

        // Load the sprite's image.
        thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                                textureExtent);

        // Add the callback.
        thumbnail.setOnSelected(
            [this, &graphicSet](AUI::Thumbnail*) {
                // This view closes immediately so we don't want to select this
                // thumbnail, but we should clear any existing selection.
                buildPanel.clearSelectedThumbnail();

                // Send a request to change the entity's animation state.
                GraphicStateChangeRequest changeRequest{
                    editingEntityID, GraphicState{graphicSet.numericID}};
                network.serializeAndSend(changeRequest);

                // Switch back to the edit view.
                changeView(ViewType::Edit);
            });

        graphicSetContainer.push_back(std::move(thumbnailPtr));
    }
}

SDL_Rect
    EntityPanelContent::calcSquareTexExtent(const SpriteRenderData& renderData)
{
    SDL_Rect textureExtent{renderData.textureExtent};
    if (textureExtent.h > textureExtent.w) {
        int diff{textureExtent.h - textureExtent.w};
        textureExtent.h -= diff;
        textureExtent.y += diff;
    }

    return textureExtent;
}

} // End namespace Client
} // End namespace AM
