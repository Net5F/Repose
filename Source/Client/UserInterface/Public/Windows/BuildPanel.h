#pragma once

#include "MainButton.h"
#include "TileLayer.h"
#include "BuildModeType.h"
#include "EntityPanelContent.h"
#include "ItemPanelContent.h"
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
class Simulation;
class World;
class Network;
class GraphicData;
class IconData;
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
    BuildPanel(Simulation& inSimulation, Network& inNetwork,
               GraphicData& inGraphicData, IconData& inIconData,
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
     * Adds a graphic set thumbnail to the appropriate tile graphic set 
     * container.
     */
    void addTileGraphicSet(TileLayer::Type type, const GraphicSet& graphicSet,
                          const Sprite& sprite);

    /**
     * Returns the first graphic within the set.
     */
    template<typename T>
    requires std::same_as<T, FloorCoveringGraphicSet> || std::same_as<
        T, ObjectGraphicSet>
    const Sprite& getFirstSprite(const T& graphicSet);

    /**
     * Sets this panel up for the given build mode type.
     */
    void setBuildMode(BuildMode::Type buildModeType);

    /** Used to send and receive content-related build mode messages. */
    Network& network;
    /** Used to get the graphic sets that we fill the panel with. */
    GraphicData& graphicData;
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

    // Item tool content panel.
    ItemPanelContent itemPanelContent;

    AUI::Text tileLayersLabel;
    AUI::Text otherLabel;

    std::array<MainButton, BuildMode::Type::Count> buildModeButtons;
};

} // End namespace Client
} // End namespace AM
