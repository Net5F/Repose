#include "ItemThumbnail.h"
#include "DragDropData.h"
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
, thumbnailImage({0, 0, logicalExtent.w, logicalExtent.h})
, selectedImage({0, 0, logicalExtent.w, logicalExtent.h})
, dragDropImage({0, 0, logicalExtent.w, logicalExtent.h})
, isHoverable{true}
, isHovered{false}
, isSelected{false}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(thumbnailImage);
    children.push_back(selectedImage);

    // Make the images we aren't using invisible.
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

void ItemThumbnail::setOnMouseDown(
    std::function<bool(ItemThumbnail*, AUI::MouseButtonType)> inOnMouseDown)
{
    onMouseDownFunc = std::move(inOnMouseDown);
}

void ItemThumbnail::setOnMouseUp(
    std::function<void(ItemThumbnail*, AUI::MouseButtonType)> inOnMouseUp)
{
    onMouseUpFunc = std::move(inOnMouseUp);
}

void ItemThumbnail::setOnDeselected(std::function<void(ItemThumbnail*)> inOnDeselected)
{
    onDeselected = std::move(inOnDeselected);
}

void ItemThumbnail::setOnDrop(
    std::function<void(const DragDropData& dragDropData)> inOnDrop)
{
    onDropFunc = std::move(inOnDrop);
}

AUI::Image* ItemThumbnail::getDragDropImage()
{
    return &dragDropImage;
}

AUI::EventResult ItemThumbnail::onMouseDown(AUI::MouseButtonType buttonType,
                                            const SDL_Point&)
{
    // If the user set a callback for this event, call it.
    bool setMouseCapture{false};
    if (onMouseDownFunc) {
        setMouseCapture = onMouseDownFunc(this, buttonType);
    }

    // If the handler says to request mouse capture, do so. This will let 
    // us receive the associated MouseUp.
    Widget* mouseCaptor{setMouseCapture ? this : nullptr};
    return AUI::EventResult{.wasHandled{true}, .setMouseCapture{mouseCaptor}};
}

AUI::EventResult ItemThumbnail::onMouseUp(AUI::MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    // Note: We have to handle clicks on mouse up since we have a drag 
    //       interaction.

    // Since we capture the mouse on MouseDown, we need to check if the 
    // cursor is actually touching this widget.
    if (!containsPoint(cursorPosition)) {
        return AUI::EventResult{.wasHandled{false}, .releaseMouseCapture{true}};
    }

    // If the user set a callback for this event, call it.
    if (onMouseUpFunc) {
        onMouseUpFunc(this, buttonType);
    }

    return AUI::EventResult{.wasHandled{false}, .releaseMouseCapture{true}};
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

AUI::EventResult ItemThumbnail::onDrop(const AUI::DragDropData& dragDropData)
{
    // Cast dragDropData to our project's type.
    // Note: If AUI ever adds support for other types, we'll need to do a check 
    //       before making this cast.
    const DragDropData& projectDragDropData{
        static_cast<const DragDropData&>(dragDropData)};

    // If the user set a callback for this event, call it.
    if (onDropFunc) {
        onDropFunc(projectDragDropData);

        return AUI::EventResult{.wasHandled{true}};
    }

    return AUI::EventResult{.wasHandled{false}};
}

void ItemThumbnail::setIsHovered(bool inIsHovered)
{
    isHovered = inIsHovered;
}

void ItemThumbnail::setIsSelected(bool inIsSelected)
{
    isSelected = inIsSelected;
    selectedImage.setIsVisible(isSelected);
}

} // namespace Client
} // namespace AM
