#include "BuildOverlay.h"
#include "SpriteData.h"
#include "Sprite.h"
#include "FloorTool.h"
#include "FloorCoveringTool.h"
#include "WallTool.h"
#include "ObjectTool.h"
#include "RemoveTool.h"
#include "Paths.h"
#include "Transforms.h"
#include "ClientTransforms.h"
#include "WorldSinks.h"
#include "QueuedEvents.h"
#include "Ignore.h"
#include "AUI/Core.h"

namespace AM
{
namespace Client
{
BuildOverlay::BuildOverlay(const World& inWorld, WorldSinks& inWorldSinks,
                           EventDispatcher& inUiEventDispatcher)
: AUI::Window({0, 0, 1920, 744}, "BuildOverlay")
, world{inWorld}
, uiEventDispatcher{inUiEventDispatcher}
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
                = std::make_unique<FloorTool>(world, uiEventDispatcher);
            break;
        }
        case BuildTool::Type::FloorCovering: {
            currentBuildTool
                = std::make_unique<FloorCoveringTool>(world, uiEventDispatcher);
            break;
        }
        case BuildTool::Type::Wall: {
            currentBuildTool
                = std::make_unique<WallTool>(world, uiEventDispatcher);
            break;
        }
        case BuildTool::Type::Object: {
            currentBuildTool
                = std::make_unique<ObjectTool>(world, uiEventDispatcher);
            break;
        }
        case BuildTool::Type::Remove: {
            currentBuildTool
                = std::make_unique<RemoveTool>(world, uiEventDispatcher);
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

void BuildOverlay::setCamera(const Camera& inCamera)
{
    camera = inCamera;

    if (currentBuildTool != nullptr) {
        currentBuildTool->setCamera(camera);
    }
}

std::span<const PhantomTileSpriteInfo> BuildOverlay::getPhantomTileSprites() const
{
    if (!isVisible) {
        // If we're not visible, don't show any phantoms.
        return {};
    }
    else if (currentBuildTool != nullptr) {
        // If we're visible and have a build tool selected, show its phantoms.
        return currentBuildTool->getPhantomTileSprites();
    }
    else {
        return {};
    }
}

std::span<const TileSpriteColorModInfo> BuildOverlay::getTileSpriteColorMods() const
{
    if (!isVisible) {
        // If we're not visible, don't show any phantoms.
        return {};
    }
    else if (currentBuildTool != nullptr) {
        // If we're visible and have a build tool selected, show its color mods.
        return currentBuildTool->getTileSpriteColorMods();
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
