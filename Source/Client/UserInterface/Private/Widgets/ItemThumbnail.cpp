#include "ItemThumbnail.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "Log.h"

namespace AM
{
namespace Client
{
ItemThumbnail::ItemThumbnail(const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, backdropImage({0, 0, logicalExtent.w, logicalExtent.h})
, thumbnailImage({0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage({0, 0, logicalExtent.w, logicalExtent.h})
, selectedImage({0, 0, logicalExtent.w, logicalExtent.h})
, isHoverable{true}
, isHovered{false}
, isSelected{false}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backdropImage);
    children.push_back(thumbnailImage);
    children.push_back(hoveredImage);
    children.push_back(selectedImage);

    // Flag ourselves as focusable, so we can auto-select.
    isFocusable = true;

    // Make the images we aren't using invisible.
    hoveredImage.setIsVisible(false);
    selectedImage.setIsVisible(false);
}

bool ItemThumbnail::getIsHovered()
{
    return isHovered;
}

bool ItemThumbnail::getIsSelected()
{
    return isSelected;
}

void ItemThumbnail::setIsHoverable(bool inIsHoverable)
{
    isHoverable = inIsHoverable;
}

void ItemThumbnail::setOnHovered(
    std::function<void(ItemThumbnail*)> inOnHovered)
{
    onHovered = std::move(inOnHovered);
}

void ItemThumbnail::setOnUnhovered(
    std::function<void(ItemThumbnail*)> inOnUnhovered)
{
    onUnhovered = std::move(inOnUnhovered);
}

void ItemThumbnail::setOnLeftClicked(std::function<void(ItemThumbnail*)> inOnLeftClicked)
{
    onLeftClicked = std::move(inOnLeftClicked);
}

void ItemThumbnail::setOnRightClicked(std::function<void(ItemThumbnail*)> inOnRightClicked)
{
    onRightClicked = std::move(inOnRightClicked);
}

void ItemThumbnail::setOnDeselected(std::function<void(ItemThumbnail*)> inOnDeselected)
{
    onDeselected = std::move(inOnDeselected);
}

AUI::EventResult ItemThumbnail::onMouseDown(AUI::MouseButtonType buttonType,
                                            const SDL_Point&)
{
    // TODO: Drag/drop

    // Set mouse capture so we get the associated MouseUp.
    return AUI::EventResult{.wasHandled{true}, .setMouseCapture{this}};
}

AUI::EventResult ItemThumbnail::onMouseUp(AUI::MouseButtonType buttonType,
                                          const SDL_Point&)
{
    // Note: We have to handle clicks on mouse up since we have a drag 
    //       interaction.

    // If the user set a callback for this event, call it.
    if ((buttonType == AUI::MouseButtonType::Left)
        && onLeftClicked) {
        onLeftClicked(this);
        return AUI::EventResult{.wasHandled{true}};
    }
    else if ((buttonType == AUI::MouseButtonType::Right)
        && onRightClicked) {
        onRightClicked(this);
        return AUI::EventResult{.wasHandled{true}};
    }

    return AUI::EventResult{.wasHandled{false}};
}

AUI::EventResult ItemThumbnail::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    return onMouseDown(buttonType, cursorPosition);
}

void ItemThumbnail::onMouseEnter()
{
    // If we're not hovered, become hovered.
    if (!isHovered) {
        setIsHovered(true);

        // If the user set a callback for this event, call it.
        if (onHovered) {
            onHovered(this);
        }
    }
}

void ItemThumbnail::onMouseLeave()
{
    // If we're hovered, unhover.
    if (isHovered) {
        setIsHovered(false);

        // If the user set a callback for this event, call it.
        if (onUnhovered) {
            onUnhovered(this);
        }
    }
}

AUI::EventResult ItemThumbnail::onFocusGained()
{
    // When we gain focus, track that we're selected.
    setIsSelected(true);

    return AUI::EventResult{.wasHandled{true}};
}

void ItemThumbnail::onFocusLost(AUI::FocusLostType focusLostType)
{
    // When we lose focus, reset our selected state.

    // If we aren't selected, do nothing.
    if (!isSelected) {
        return;
    }

    setIsSelected(false);

    // If the user set a callback for this event, call it.
    if (onDeselected) {
        onDeselected(this);
    }
}

void ItemThumbnail::setIsHovered(bool inIsHovered)
{
    isHovered = inIsHovered;
    hoveredImage.setIsVisible(isHovered);
}

void ItemThumbnail::setIsSelected(bool inIsSelected)
{
    isSelected = inIsSelected;
    selectedImage.setIsVisible(isSelected);
}

} // namespace Client
} // namespace AM
