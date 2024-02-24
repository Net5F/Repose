#include "BuildOverlay.h"
#include "Simulation.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "GraphicData.h"
#include "Sprite.h"
#include "BuildTool.h"
#include "FloorTool.h"
#include "FloorCoveringTool.h"
#include "WallTool.h"
#include "ObjectTool.h"
#include "EntityTool.h"
#include "RemoveTool.h"
#include "Paths.h"
#include "Transforms.h"
#include "ClientTransforms.h"
#include "QueuedEvents.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client
{
BuildOverlay::BuildOverlay(Simulation& inSimulation,
                           const WorldObjectLocator& inWorldObjectLocator,
                           Network& inNetwork, GraphicData& inGraphicData)
: AUI::Window({0, 0, 1920, 744}, "BuildOverlay")
, world{inSimulation.getWorld()}
, worldObjectLocator{inWorldObjectLocator}
, network{inNetwork}
, graphicData{inGraphicData}
, selectedGraphicSet{nullptr}
, currentBuildTool{nullptr}
, camera{}
, mapTileExtent{}
, cursorTilePosition{}
{
    // We need to know when the map size changes so we can bound the cursor
    // appropriately.
    world.tileMap.sizeChanged.connect<&BuildOverlay::onTileMapExtentChanged>(
        *this);
}

BuildOverlay::~BuildOverlay() = default;

void BuildOverlay::setSelectedGraphicSet(const GraphicSet& inSelectedGraphicSet)
{
    selectedGraphicSet = &inSelectedGraphicSet;

    if (currentBuildTool != nullptr) {
        currentBuildTool->setSelectedGraphicSet(*selectedGraphicSet);
    }
}

void BuildOverlay::setBuildMode(BuildMode::Type buildModeType)
{
    switch (buildModeType) {
        case BuildMode::Type::Floor: {
            currentBuildTool = std::make_unique<FloorTool>(world, network);
            break;
        }
        case BuildMode::Type::FloorCovering: {
            currentBuildTool
                = std::make_unique<FloorCoveringTool>(world, network);
            break;
        }
        case BuildMode::Type::Wall: {
            currentBuildTool = std::make_unique<WallTool>(world, network);
            break;
        }
        case BuildMode::Type::Object: {
            currentBuildTool = std::make_unique<ObjectTool>(world, network);
            break;
        }
        case BuildMode::Type::Entity: {
            currentBuildTool = std::make_unique<EntityTool>(
                world, worldObjectLocator, network, graphicData);
            break;
        }
        case BuildMode::Type::Remove: {
            currentBuildTool = std::make_unique<RemoveTool>(
                world, worldObjectLocator, network);
            break;
        }
        case BuildMode::Type::Item: {
            // Item build mode doesn't have any overlay interactions.
            currentBuildTool = nullptr;
            break;
        }
        default: {
            LOG_FATAL("Invalid tool type.");
            break;
        }
    }

    if (currentBuildTool != nullptr) {
        currentBuildTool->setCamera(camera);
        currentBuildTool->setTileMapExtent(mapTileExtent);
    }
}

BuildTool* BuildOverlay::getCurrentBuildTool()
{
    return currentBuildTool.get();
}

void BuildOverlay::setCamera(const Camera& inCamera)
{
    camera = inCamera;

    if (currentBuildTool != nullptr) {
        currentBuildTool->setCamera(camera);
    }
}

std::span<const PhantomSpriteInfo> BuildOverlay::getPhantomSprites() const
{
    if (!isVisible) {
        // If we're not visible, don't show any phantoms.
        return {};
    }
    else if (currentBuildTool != nullptr) {
        // If we're visible and have a build tool selected, show its phantoms.
        return currentBuildTool->getPhantomSprites();
    }
    else {
        return {};
    }
}

std::span<const SpriteColorModInfo> BuildOverlay::getSpriteColorMods() const
{
    if (!isVisible) {
        // If we're not visible, don't show any phantoms.
        return {};
    }
    else if (currentBuildTool != nullptr) {
        // If we're visible and have a build tool selected, show its color mods.
        return currentBuildTool->getSpriteColorMods();
    }
    else {
        return {};
    }
}

void BuildOverlay::render()
{
    // TODO: This will be used to render highlight rectangles from the build
    //       tool.
}

AUI::EventResult BuildOverlay::onMouseDown(AUI::MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition)
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseDown(buttonType, cursorPosition);
        return AUI::EventResult{.wasHandled{true}};
    }

    return AUI::EventResult{.wasHandled{true}};
}

AUI::EventResult BuildOverlay::onMouseUp(AUI::MouseButtonType buttonType,
                                         const SDL_Point& cursorPosition)
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseUp(buttonType, cursorPosition);
    }

    return AUI::EventResult{.wasHandled{true}};
}

AUI::EventResult
    BuildOverlay::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                     const SDL_Point& cursorPosition)
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseDoubleClick(buttonType, cursorPosition);
    }

    return AUI::EventResult{.wasHandled{true}};
}

AUI::EventResult BuildOverlay::onMouseWheel(int amountScrolled)
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseWheel(amountScrolled);
    }

    return AUI::EventResult{.wasHandled{true}};
}

AUI::EventResult BuildOverlay::onMouseMove(const SDL_Point& cursorPosition)
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseMove(cursorPosition);
    }

    return AUI::EventResult{.wasHandled{true}};
}

void BuildOverlay::onMouseLeave()
{
    if (currentBuildTool != nullptr) {
        currentBuildTool->onMouseLeave();
    }
}

void BuildOverlay::onTileMapExtentChanged(TileExtent inTileExtent)
{
    mapTileExtent = inTileExtent;

    if (currentBuildTool != nullptr) {
        currentBuildTool->setTileMapExtent(mapTileExtent);
    }
}

} // End namespace Client
} // End namespace AM
