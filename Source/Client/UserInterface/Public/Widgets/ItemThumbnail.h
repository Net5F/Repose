#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AM
{
namespace Client
{
/**
 * An item thumbnail for use in the player's inventory, a paper doll equipment 
 * menu, etc.
 *
 * Can be left and right-clicked. Can also be dragged and dropped.
 *
 * This widget's selection state is a bit odd: to select the widget, set focus 
 * to it using Screen::setFocus() (it'll update its own selection state).
 * You don't need to manually remove focus though, this widget will instead 
 * drop focus and deselect itself when appropriate.
 * This was done because selection may come from other widgets (e.g. a right-
 * click menu), but deselection is easiest to detect internally.
 * 
 * The rendering order for this widget's children is:
 *   Background: backdropImage
 *   Middle-ground: thumbnailImage
 *   Foreground: hoveredImage, selectedImage
 *
 * When dragged, only thumbnailImage follows the mouse.
 */
class ItemThumbnail : public AUI::Widget
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    ItemThumbnail(const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "ItemThumbnail");

    virtual ~ItemThumbnail() = default;

    bool getIsHovered();
    bool getIsSelected();

    /** If true, this widget displays hoveredImage when hovered. */
    void setIsHoverable(bool inIsHoverable);

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Thumbnail image. */
    AUI::Image thumbnailImage;

    // Note: Hover highlight isn't needed, since we have interaction text 
    //       providing hover feedback.

    /** Foreground highlight image, selected state. */
    AUI::Image selectedImage;

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    /**
     * @param inOnHovered  A callback that expects a pointer to this widget.
     */
    void setOnHovered(std::function<void(ItemThumbnail*)> inOnHovered);

    void setOnUnhovered(std::function<void(ItemThumbnail*)> inOnUnhovered);

    /**
     * @param inOnMouseDown A callback that returns true if the mouse should be
     *        captured, else false.
     */
    void
        setOnMouseDown(std::function<bool(ItemThumbnail*, AUI::MouseButtonType)>
                           inOnMouseDown);

    void setOnMouseUp(
        std::function<void(ItemThumbnail*, AUI::MouseButtonType)> inOnMouseUp);

    void setOnDeselected(std::function<void(ItemThumbnail*)> inOnDeselected);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    AUI::Image* getDragDropImage() override;

    AUI::EventResult onMouseDown(AUI::MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

    AUI::EventResult onMouseUp(AUI::MouseButtonType buttonType,
                               const SDL_Point& cursorPosition) override;

    AUI::EventResult onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition) override;

    void onMouseEnter() override;

    void onMouseLeave() override;

    AUI::EventResult onFocusGained();

    void onFocusLost(AUI::FocusLostType focusLostType);

    AUI::EventResult onDrop(const AUI::DragDropData& dragDropData) override;

private:
    /** Sets isHovered and updates the visibility of hoveredImage. */
    void setIsHovered(bool inIsHovered);
    /** Sets isSelected and updates the visibility of selectedImage. */
    void setIsSelected(bool inIsSelected);

    std::function<void(ItemThumbnail*)> onHovered;
    std::function<void(ItemThumbnail*)> onUnhovered;
    std::function<bool(ItemThumbnail*, AUI::MouseButtonType)> onMouseDownFunc;
    std::function<void(ItemThumbnail*, AUI::MouseButtonType)> onMouseUpFunc;
    std::function<void(ItemThumbnail*)> onDeselected;

    /** If true, this widget displays hoveredImage when hovered. */
    bool isHoverable;

    /** Tracks whether the mouse is currently hovering over this widget. */
    bool isHovered;

    /** Tracks whether this widget is currently selected. */
    bool isSelected;
};

} // namespace Client
} // namespace AM
