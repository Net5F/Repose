#pragma once

#include "MainButton.h"
#include "AUI/Widget.h"
#include "AUI/Text.h"
#include "AUI/TextInput.h"
#include "AUI/VerticalGridContainer.h"
#include "entt/fwd.hpp"

namespace AM
{
namespace Client
{
/**
 * Content for the BuildPanel when the DynamicObject tool is selected.
 */
class DynamicObjectPanelContent : public AUI::Widget
{
public:
    DynamicObjectPanelContent(const SDL_Rect& inScreenExtent,
               const std::string& inDebugName = "ObjectEntityPanelContent");

    void openEditView(entt::entity inObjectToEdit);

    void closeEditView();

private:
    // Normal view
    /** Holds the templates and the "New Object Entity" button. */
    AUI::VerticalGridContainer templateContainer;

    // Edit view
    AUI::Text nameLabel;

    AUI::TextInput nameInput;

    MainButton changeSpriteButton;

    MainButton changeScriptButton;

    MainButton saveTemplateButton;

    /** Holds the sprites that the object can be set to use. */
    AUI::VerticalGridContainer spriteContainer;

    /** The object entity that we're currently editing. */
    entt::entity objectToEdit;
};

} // End namespace Client
} // End namespace AM
