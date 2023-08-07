#include "EntityPanelContent.h"
#include "SpriteData.h"
#include "BuildPanel.h"
#include "MainThumbnail.h"
#include "EntityTool.h"
#include "Paths.h"
#include "entt/entity/entity.hpp"

namespace AM
{
namespace Client
{
EntityPanelContent::EntityPanelContent(
    EventDispatcher& inNetworkEventDispatcher, SpriteData& inSpriteData,
    BuildPanel& inBuildPanel, const SDL_Rect& inScreenExtent,
    const std::string& inDebugName)
: AUI::Widget(inScreenExtent, inDebugName)
, spriteData{inSpriteData}
, buildPanel{inBuildPanel}
, entityTool{nullptr}
, editingEntityID{entt::null}
, selectedSpriteThumbnail{nullptr}
, entityTemplatesQueue{inNetworkEventDispatcher}
// Note: These dimensions are based on the top left that BuildPanel gives us.
, templateContainer{{0, 0, logicalExtent.w, logicalExtent.h},
                    "TemplateContainer"}
, nameLabel{{526, 1, 138, 36}, "NameLabel"}
, nameInput{{468, 38, 255, 42}, "NameInput"}
, changeScriptButton{{518, 113, 156, 36}, "Change Script", "ChangeScriptButton"}
, saveTemplateButton{{724, 113, 188, 36},
                     "Save as Template",
                     "SaveTemplateButton"}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(templateContainer);
    children.push_back(nameLabel);
    children.push_back(nameInput);
    children.push_back(changeScriptButton);
    children.push_back(saveTemplateButton);

    /* Label */
    auto setTextStyle = [](AUI::Text& text) {
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
        text.setColor({255, 255, 255, 255});
        text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
        text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    };
    setTextStyle(nameLabel);
    nameLabel.setText("Name");

    /* Text input */
    nameInput.normalImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
    nameInput.hoveredImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Hovered.png"), {8, 8, 8, 8});
    nameInput.focusedImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Focused.png"), {8, 8, 8, 8});
    nameInput.setTextFont(((Paths::FONT_DIR + "Cagliostro-Regular.ttf")), 20);
    nameInput.setTextColor({255, 255, 255, 255});
    nameInput.setPadding({0, 8, 0, 8});
    nameInput.setCursorWidth(2);
    nameInput.setCursorColor({255, 255, 255, 255});

    nameInput.setOnTextCommitted([this]() {
        // TODO: Send a "change name" or "update entity" or whatever
    });

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
    };
    setContainerStyle(templateContainer);

    // Hide the edit view widgets.
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    changeScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);

    /* Buttons */
    changeScriptButton.setOnPressed([this]() {
        // TODO: Open the file picker and send the change message
    });

    saveTemplateButton.setOnPressed([this]() {
        // TODO: Send a save entity template message
    });

    // Add the thumbnails that we can (we get some later from the server).
    addAddThumbnail();
}

void EntityPanelContent::setBuildTool(
    EntityTool* inEntityTool)
{
    entityTool = inEntityTool;

    // Register our callbacks.
    if (entityTool != nullptr) {
        entityTool->setOnSelectionCleared(
            [this]() { buildPanel.clearSelectedThumbnail(); });
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
}

void EntityPanelContent::setObjectToEdit(
    entt::entity ID, std::string name, const ObjectSpriteSet& spriteSet,
    Uint8 spriteIndex)
{
    editingEntityID = ID;
    editingEntityName = name;
    editingEntitySpriteSet = &spriteSet;
    editingEntitySpriteIndex = spriteIndex;

    // Set the name input's text to match the new object.
    nameInput.setText(name);
}

void EntityPanelContent::openEditView()
{
    // Make all the normal view components invisible.
    templateContainer.setIsVisible(false);

    // Make all the edit view components visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    changeScriptButton.setIsVisible(true);
    saveTemplateButton.setIsVisible(true);
}

void EntityPanelContent::closeEditView()
{
    // Make all the edit view components invisible.
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    changeScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);

    // Make all the normal view components visible.
    templateContainer.setIsVisible(true);
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
        entityTool->setSelectedEntity(
            "NewEntity",
            spriteData.getObjectSpriteSet(
                SharedConfig::DEFAULT_ENTITY_SPRITE_SET),
            SharedConfig::DEFAULT_ENTITY_SPRITE_INDEX);
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

        // Get the object's default sprite.
        const ObjectSpriteSet& spriteSet{
            spriteData.getObjectSpriteSet(entityData.spriteSetID)};
        const Sprite* sprite{spriteSet.sprites[entityData.spriteIndex]};

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
        thumbnail.setOnSelected([this, &spriteSet,
                                 entityData](AUI::Thumbnail* selectedThumb) {
            // Set this thumbnail as the new selection.
            buildPanel.setSelectedThumbnail(*selectedThumb);

            // Tell the tool that the selection changed.
            entityTool->setSelectedEntity(
                entityData.name, spriteSet, entityData.spriteIndex);
        });

        templateContainer.push_back(std::move(thumbnailPtr));
    }
}

} // End namespace Client
} // End namespace AM
