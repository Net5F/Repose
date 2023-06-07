#pragma once

#include "MainButton.h"
#include "TileLayers.h"
#include "BuildTool.h"
#include "Log.h"
#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/VerticalGridContainer.h"
#include "AUI/Text.h"
#include <concepts>

namespace AUI
{
class Thumbnail;
}

namespace AM
{
struct Sprite;

namespace Client
{
class SpriteData;
class BuildOverlay;

/**
 * The build panel on the main screen. Allows the user to select which tile
 * they want to add to the world.
 *
 * This panel is opened alongside BuildOverlay when we enter build mode.
 */
class BuildPanel : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    BuildPanel(SpriteData& inSpriteData, BuildOverlay& inBuildOverlay);

    ~BuildPanel() = default;

private:
    /**
     * Adds a sprite set thumbnail to the appropriate tileSpriteSetContainer.
     */
    void addSpriteSet(TileLayer::Type type, const SpriteSet& spriteSet,
                      const Sprite& sprite);

    /**
     * Returns the first sprite within the set.
     */
    template<typename T> requires std::same_as<T, FloorCoveringSpriteSet>
                                  || std::same_as<T, ObjectSpriteSet>
    const Sprite* getFirstSprite(const T& spriteSet);

    /**
     * Sets the current build tool to the given tool type.
     */
    void setBuildTool(BuildTool::Type toolType);

    /** Used to get the sprite sets that we fill the panel with. */
    SpriteData& spriteData;

    /** We keep the overlay updated on which tool and sprite set is selected. */
    BuildOverlay& buildOverlay;

    /** The currently selected sprite set thumbnail. */
    AUI::Thumbnail* selectedThumbnail;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    /** The containers that hold the sprite sets that the user can select.
        Only one of these containers is visible at a time, based on which mode 
        the user has selected.
        Note: We use TileLayer::Count instead of SpriteSet::Count because 
              tiles don't support every type of sprite set. */
    std::array<AUI::VerticalGridContainer, TileLayer::Type::Count>
        tileSpriteSetContainers;

    AUI::Text toolsLabel;

    std::array<MainButton, BuildTool::Type::Count> buildToolButtons;
};

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

} // End namespace Client
} // End namespace AM
