#include "EntityPanelContent.h"
#include "World.h"
#include "Network.h"
#include "SpriteData.h"
#include "BuildPanel.h"
#include "MainThumbnail.h"
#include "EntityTool.h"
#include "Name.h"
#include "Position.h"
#include "Rotation.h"
#include "AnimationState.h"
#include "EntityInitRequest.h"
#include "ComponentUpdateRequest.h"
#include "InitScriptRequest.h"
#include "Paths.h"
#include "AMAssert.h"
#include "entt/entity/entity.hpp"
#include <fstream>

namespace AM
{
namespace Client
{
EntityPanelContent::EntityPanelContent(
    World& inWorld, Network& inNetwork, SpriteData& inSpriteData,
    BuildPanel& inBuildPanel, const SDL_Rect& inScreenExtent,
    const std::string& inDebugName)
: AUI::Widget(inScreenExtent, inDebugName)
, world{inWorld}
, network{inNetwork}
, spriteData{inSpriteData}
, buildPanel{inBuildPanel}
, currentView{ViewType::Template}
, entityTool{nullptr}
, editingEntityID{entt::null}
, editingEntityInitScript{""}
, selectedSpriteThumbnail{nullptr}
, entityTemplatesQueue{inNetwork.getEventDispatcher()}
, initScriptQueue{inNetwork.getEventDispatcher()}
// Note: These dimensions are based on the top left that BuildPanel gives us.
, templateContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                    "TemplateContainer"}
, nameLabel{{526, 1, 138, 36}, "NameLabel"}
, nameInput{{468, 38, 255, 42}, "NameInput"}
, changeSpriteButton{{308, 113, 160, 36}, "Change Sprite", "ChangeSpriteButton"}
, changeScriptButton{{518, 113, 156, 36}, "Change Script", "ChangeScriptButton"}
, saveTemplateButton{{724, 113, 188, 36},
                     "Save as Template",
                     "SaveTemplateButton"}
, spriteSetContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                    "SpriteSetContainer"}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(templateContainer);
    children.push_back(nameLabel);
    children.push_back(nameInput);
    children.push_back(changeSpriteButton);
    children.push_back(changeScriptButton);
    children.push_back(saveTemplateButton);
    children.push_back(spriteSetContainer);

    /* Label */
    auto setTextStyle = [](AUI::Text& text) {
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
        text.setColor({255, 255, 255, 255});
        text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
        text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    };
    setTextStyle(nameLabel);
    nameLabel.setText("Name");

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
    };
    setContainerStyle(templateContainer);
    setContainerStyle(spriteSetContainer);

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
        // Send a re-init request with the updated name.
        const entt::registry& registry{world.registry};
        EntityInitRequest initRequest{editingEntityID};
        initRequest.components.push_back(
            registry.get<Position>(editingEntityID));
        initRequest.components.push_back(
            registry.get<AnimationState>(editingEntityID));
        initRequest.components.push_back(Name{nameInput.getText()});
        initRequest.initScript = editingEntityInitScript;
        network.serializeAndSend(initRequest);
    });

    /* Buttons */
    changeSpriteButton.setOnPressed(
        [this]() { changeView(ViewType::SpriteSet); });

    changeScriptButton.setOnPressed([this]() {
        // Send a re-init request with the updated script.
        const entt::registry& registry{world.registry};
        EntityInitRequest initRequest{editingEntityID};
        initRequest.components.push_back(
            registry.get<Name>(editingEntityID));
        initRequest.components.push_back(
            registry.get<Position>(editingEntityID));
        initRequest.components.push_back(
            registry.get<AnimationState>(editingEntityID));
        std::string initScript{""};
        // TEMP
        std::ifstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
        if (scriptFile.is_open()) {
            std::stringstream buffer;
            buffer << scriptFile.rdbuf();
            initScript = buffer.str();
        }
        // TEMP
        initRequest.initScript = initScript;
        network.serializeAndSend(initRequest);
    });

    saveTemplateButton.setOnPressed([this]() {
        // TODO: Send an add entity template message
    });

    // Add the thumbnails that we can (we get some later from the server).
    addAddThumbnail();
    addSpriteSetThumbnails();

    // Hide any non-template-view widgets.
    changeView(ViewType::Template);
}

void EntityPanelContent::setBuildTool(EntityTool* inEntityTool)
{
    entityTool = inEntityTool;

    // Register our callbacks.
    if (entityTool != nullptr) {
        entityTool->setOnEntitySelected(
            [this](entt::entity entity) {
                // Save the entity's data.
                editingEntityID = entity;

                // Ask the server for the entity's init script.
                network.serializeAndSend(InitScriptRequest{editingEntityID});

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

void EntityPanelContent::onTick(double timestepS)
{
    // Process any waiting messages.
    EntityTemplates entityTemplates{};
    while (entityTemplatesQueue.pop(entityTemplates)) {
        // Clear any existing entity templates (skipping the "add entity" 
        // thumbnail).
        if (templateContainer.size() > 1) {
            templateContainer.erase(templateContainer.begin() + 1,
                                    templateContainer.end());
        }

        addTemplateThumbnails(entityTemplates);
    }

    InitScriptResponse initScriptResponse{};
    while (initScriptQueue.pop(initScriptResponse)) {
        // If the received script is for the currently selected entity, save it.
        if (initScriptResponse.entity == editingEntityID) {
            editingEntityInitScript = initScriptResponse.initScript;

            // TEMP
            // Write to CurrentScript.lua
            std::ofstream scriptFile{Paths::BASE_PATH + "InitScript.lua"};
            scriptFile << initScriptResponse.initScript;
            LOG_INFO("Received script. Saved to InitScript.lua");
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
    changeScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);
    spriteSetContainer.setIsVisible(false);

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
        changeScriptButton.setIsVisible(true);
        saveTemplateButton.setIsVisible(true);
    }
    else if (newView == ViewType::SpriteSet) {
        // TODO: Select the correct thumbnail
        spriteSetContainer.setIsVisible(true);
    }

    currentView = newView;
}

void EntityPanelContent::addAddThumbnail()
{
    // Construct the new thumbnail.
    std::unique_ptr<AUI::Widget> thumbnailPtr{
        std::make_unique<MainThumbnail>("EntityThumbnail")};
    MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
    thumbnail.setText("");
    thumbnail.setIsActivateable(false);

    // Load the "add new entity" image.
    thumbnail.thumbnailImage.setSimpleImage(Paths::TEXTURE_DIR
                                            + "BuildPanel/EraserIcon_1600.png");

    // Add the callback.
    thumbnail.setOnSelected([this](AUI::Thumbnail* selectedThumb) {
        // Set this thumbnail as the new selection.
        buildPanel.setSelectedThumbnail(*selectedThumb);

        // Tell the tool that the selection changed.
        const SpriteSet& spriteSet{spriteData.getObjectSpriteSet(
                SharedConfig::DEFAULT_DYNAMIC_OBJECT_SPRITE_SET)};
        AnimationState animationState{
            SpriteSet::Type::Object, spriteSet.numericID,
            SharedConfig::DEFAULT_DYNAMIC_OBJECT_SPRITE_INDEX};
        entityTool->setSelectedTemplate({"NewEntity"}, animationState);
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
            std::make_unique<MainThumbnail>("EntityThumbnail")};
        MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Get the sprite.
        const ObjectSpriteSet& spriteSet{spriteData.getObjectSpriteSet(
            entityData.animationState.spriteSetID)};
        const Sprite* sprite{
            spriteSet.sprites[entityData.animationState.spriteIndex]};

        // Calc a square texture extent that shows the bottom of the sprite 
        // (so we don't have to squash it).
        SpriteRenderData renderData{
            spriteData.getRenderData(sprite->numericID)};
        SDL_Rect textureExtent{renderData.textureExtent};
        if (textureExtent.h > textureExtent.w) {
            int diff{textureExtent.h - textureExtent.w};
            textureExtent.h -= diff;
            textureExtent.y += diff;
        }

        // Load the sprite's image.
        thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                                textureExtent);

        // Add the callback.
        thumbnail.setOnSelected([this,
                                 entityData](AUI::Thumbnail* selectedThumb) {
            // Set this thumbnail as the new selection.
            buildPanel.setSelectedThumbnail(*selectedThumb);

            // Tell the tool that the selection changed.
            entityTool->setSelectedTemplate(
                entityData.name, entityData.animationState);
        });

        templateContainer.push_back(std::move(thumbnailPtr));
    }
}

void EntityPanelContent::addSpriteSetThumbnails()
{
    // Add thumbnails for all object sprite sets.
    for (const ObjectSpriteSet& spriteSet :
         spriteData.getAllObjectSpriteSets()) {
        // Construct the new thumbnail.
        std::unique_ptr<AUI::Widget> thumbnailPtr{
            std::make_unique<MainThumbnail>("EntityThumbnail")};
        MainThumbnail& thumbnail{static_cast<MainThumbnail&>(*thumbnailPtr)};
        thumbnail.setText("");
        thumbnail.setIsActivateable(false);

        // Find the first sprite in the set.
        const Sprite* sprite{nullptr};
        for (const Sprite* spritePtr : spriteSet.sprites) {
            if (spritePtr != nullptr) {
                sprite = spritePtr;
            }
        }
        AM_ASSERT(sprite != nullptr, "No valid sprite found in set.");

        // Calc a square texture extent that shows the bottom of the sprite 
        // (so we don't have to squash it).
        SpriteRenderData renderData{
            spriteData.getRenderData(sprite->numericID)};
        SDL_Rect textureExtent{renderData.textureExtent};
        if (textureExtent.h > textureExtent.w) {
            int diff{textureExtent.h - textureExtent.w};
            textureExtent.h -= diff;
            textureExtent.y += diff;
        }

        // Load the sprite's image.
        thumbnail.thumbnailImage.setSimpleImage(renderData.spriteSheetRelPath,
                                                textureExtent);

        // Find the first non-empty slot in the sprite set.
        Uint8 firstSpriteIndex{SDL_MAX_UINT8};
        for (std::size_t i = 0; i < spriteSet.sprites.size(); ++i) {
            if (spriteSet.sprites[i] != nullptr) {
                firstSpriteIndex = static_cast<Uint8>(i);
            }
        }
        AM_ASSERT(firstSpriteIndex != SDL_MAX_UINT8,
                  "Set didn't contain any sprites.");

        // Add the callback.
        thumbnail.setOnSelected([this, &spriteSet,
                                 firstSpriteIndex](AUI::Thumbnail* selectedThumb) {
            // This view closes immediately so we don't want to select this 
            // thumbnail, but we should clear any existing selection.
            buildPanel.clearSelectedThumbnail();

            // Tell the server to change the entity's sprite.
            ComponentUpdateRequest updateRequest{editingEntityID};
            updateRequest.components.push_back(
                AnimationState{SpriteSet::Type::Object, spriteSet.numericID,
                               firstSpriteIndex});
            network.serializeAndSend(updateRequest);

            // Switch back to the edit view.
            changeView(ViewType::Edit);
        });

        spriteSetContainer.push_back(std::move(thumbnailPtr));
    }
}

} // End namespace Client
} // End namespace AM
