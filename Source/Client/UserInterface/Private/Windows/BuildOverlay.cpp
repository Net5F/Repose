#include "BuildOverlay.h"
#include "WorldSinks.h"
#include "WorldObjectLocator.h"
#include "SpriteData.h"
#include "Sprite.h"
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
#include "Ignore.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client
{
BuildOverlay::BuildOverlay(const World& inWorld, WorldSinks& inWorldSinks,
                           const WorldObjectLocator& inWorldObjectLocator,
                           Network& inNetwork)
: AUI::Window({0, 0, 1920, 744}, "BuildOverlay")
, world{inWorld}
, worldObjectLocator{inWorldObjectLocator}
, network{inNetwork}
, selectedSpriteSet{nullptr}
, currentBuildTool{nullptr}
, camera{}
, mapTileExtent{}
, cursorTilePosition{}
{
    // We need to know when the map size changes so we can bound the cursor
    // appropriately.
    inWorldSinks.tileMapExtentChanged
        .connect<&BuildOverlay::onTileMapExtentChanged>(*this);
}

void BuildOverlay::setSelectedSpriteSet(const SpriteSet& inSelectedSpriteSet)
{
    selectedSpriteSet = &inSelectedSpriteSet;

    if (currentBuildTool != nullptr) {
        currentBuildTool->setSelectedSpriteSet(*selectedSpriteSet);
    }
}

void BuildOverlay::setBuildTool(BuildTool::Type toolType)
{
    switch (toolType) {
        case BuildTool::Type::Floor: {
            currentBuildTool
                = std::make_unique<FloorTool>(world, network);
            break;
        }
        case BuildTool::Type::FloorCovering: {
            currentBuildTool = std::make_unique<FloorCoveringTool>(
                world, network);
            break;
        }
        case BuildTool::Type::Wall: {
            currentBuildTool
                = std::make_unique<WallTool>(world, network);
            break;
        }
        case BuildTool::Type::Object: {
            currentBuildTool
                = std::make_unique<ObjectTool>(world, network);
            break;
        }
        case BuildTool::Type::Entity: {
            currentBuildTool = std::make_unique<EntityTool>(
                world, worldObjectLocator, network);
            break;
        }
        case BuildTool::Type::Remove: {
            currentBuildTool = std::make_unique<RemoveTool>(
                world, worldObjectLocator, network);
            break;
        }
        default: {
            LOG_FATAL("Invalid tool type.");
            break;
        }
    }

    currentBuildTool->setCamera(camera);
    currentBuildTool->setTileMapExtent(mapTileExtent);
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
