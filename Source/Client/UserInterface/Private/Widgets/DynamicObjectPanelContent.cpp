#include "DynamicObjectPanelContent.h"
#include "Paths.h"
#include "entt/entity/entity.hpp"

namespace AM
{
namespace Client
{
DynamicObjectPanelContent::DynamicObjectPanelContent(const SDL_Rect& inScreenExtent,
                       const std::string& inDebugName)
: AUI::Widget(inScreenExtent, inDebugName)
, templateContainer{{366 - 2, 91, 1188, 220}, "TemplateContainer"}
, nameLabel{{890, 92, 138, 36}, "NameLabel"}
, nameInput{{832, 132, 255, 36}, "NameInput"}
, changeSpriteButton{{672, 204, 160, 36}, "Change Sprite", "ChangeSpriteButton"}
, changeScriptButton{{882, 132, 156, 36}, "Change Script", "ChangeScriptButton"}
, saveTemplateButton{{1088, 132, 188, 36},
                     "Save as Template",
                     "SaveTemplateButton"}
, spriteContainer{{366 - 2, 91, 1188, 220}, "SpriteContainer"}
, objectToEdit{entt::null}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(templateContainer);
    children.push_back(nameLabel);
    children.push_back(nameInput);
    children.push_back(changeSpriteButton);
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

    /* Text input */
    nameInput.normalImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {4, 4, 4, 4});
    nameInput.hoveredImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Hovered.png"), {4, 4, 4, 4});
    nameInput.focusedImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Focused.png"), {4, 4, 4, 4});
    nameInput.setTextFont(((Paths::FONT_DIR + "Cagliostro-Regular.ttf")), 20);
    nameInput.setCursorWidth(2);

    /* Containers */
    auto setContainerStyle = [](AUI::VerticalGridContainer& container) {
        container.setNumColumns(11);
        container.setCellWidth(108);
        container.setCellHeight(109 + 1);
        container.setIsVisible(false);
    };
    setContainerStyle(templateContainer);
    setContainerStyle(spriteContainer);

    // Hide the edit view widgets.
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    changeSpriteButton.setIsVisible(false);
    changeScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);
    spriteContainer.setIsVisible(false);
}

void DynamicObjectPanelContent::openEditView(entt::entity inObjectToEdit)
{
    // TODO: Get this entity's relevant components
    objectToEdit = inObjectToEdit;

    // Make all the normal view components invisible.
    templateContainer.setIsVisible(false);

    // Make all the edit view components visible.
    nameLabel.setIsVisible(true);
    nameInput.setIsVisible(true);
    changeSpriteButton.setIsVisible(true);
    changeScriptButton.setIsVisible(true);
    saveTemplateButton.setIsVisible(true);
    spriteContainer.setIsVisible(true);
}

void DynamicObjectPanelContent::closeEditView()
{
    // Make all the edit view components invisible.
    nameLabel.setIsVisible(false);
    nameInput.setIsVisible(false);
    changeSpriteButton.setIsVisible(false);
    changeScriptButton.setIsVisible(false);
    saveTemplateButton.setIsVisible(false);
    spriteContainer.setIsVisible(false);

    // Make all the normal view components visible.
    templateContainer.setIsVisible(true);

    // Reset our tracked object.
    objectToEdit = entt::null;
}

} // End namespace Client
} // End namespace AM
