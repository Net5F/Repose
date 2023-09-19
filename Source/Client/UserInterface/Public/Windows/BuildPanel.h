#pragma once

#include "MainButton.h"
#include "TileLayers.h"
#include "BuildTool.h"
#include "EntityPanelContent.h"
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
class World;
class Network;
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
    BuildPanel(World& inWorld, Network& inNetwork, SpriteData& inSpriteData,
               BuildOverlay& inBuildOverlay);

    ~BuildPanel() = default;

    /**
     * Deselects the currently selected thumbnail (if there is one) and saves 
     * the given thumbnail as the new selection.
     * Used by content classes to make sure the old selection gets deselected 
     * when we change tools.
     */
    void setSelectedThumbnail(AUI::Thumbnail& newSelectedThumbnail);

    /**
     * Deselects the currently selected thumbnail (if there is one) without 
     * setting a new selection.
     */
    void clearSelectedThumbnail();

private:
    /**
     * Adds a sprite set thumbnail to the appropriate tileSpriteSetContainer.
     */
    void addTileSpriteSet(TileLayer::Type type, const SpriteSet& spriteSet,
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

    /** Used to send and receive content-related build mode messages. */
    Network& network;
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

    // Note: Since all of the tile layer tools have the same UI, we handle their 
    //       content in this class. Other tools get their own content widgets.
    // Floor tile layer tool content.
    AUI::VerticalGridContainer floorContainer;

    // Floor covering tile layer tool content.
    AUI::VerticalGridContainer floorCoveringContainer;

    // Wall tile layer tool content.
    AUI::VerticalGridContainer wallContainer;

    // Object tile layer tool content.
    AUI::VerticalGridContainer objectContainer;

    // Entity tool content panel.
    EntityPanelContent entityPanelContent;

    // Remove tool content.
    AUI::Text removeHintText;

    AUI::Text tileLayersLabel;
    AUI::Text entitiesLabel;
    AUI::Text generalLabel;

    std::array<MainButton, BuildTool::Type::Count> buildToolButtons;
};

} // End namespace Client
} // End namespace AM
