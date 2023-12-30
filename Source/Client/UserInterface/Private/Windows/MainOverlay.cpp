#include "MainOverlay.h"
#include "World.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "InteractionManager.h"
#include "entt/entity/entity.hpp"
#include "Paths.h"

namespace AM
{
namespace Client
{
MainOverlay::MainOverlay(World& inWorld,
                         const WorldObjectLocator& inWorldObjectLocator,
                         Network& inNetwork,
                         InteractionManager& inInteractionManager)
: AUI::Window({0, 0, 1920, 1080}, "MainOverlay")
, world{inWorld}
, worldObjectLocator{inWorldObjectLocator}
, network{inNetwork}
, interactionManager{inInteractionManager}
, hoveredEntity{entt::null}
, interactionText{{20, 20, 700, 100}, "InteractionText"}
, buildModeHintText({50, 850, 500, 500}, "BuildModeHintText")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(interactionText);
    children.push_back(buildModeHintText);

    /* Interaction text. */
    interactionText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    interactionText.setColor({255, 255, 255, 255});
    interactionText.setIsVisible(false);

    /* Build mode text. */
    buildModeHintText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    buildModeHintText.setColor({255, 255, 255, 255});
    buildModeHintText.setText("Press 'b' to enter Build Mode.");
    buildModeHintText.setIsVisible(false);

    // Update interactionText when InteractionManager tells us to.
    interactionManager.setOnInteractionTextUpdated(
        [&](std::string_view newInteractionText) {
            if (newInteractionText != "") {
                interactionText.setText(newInteractionText);
                interactionText.setIsVisible(true);
            }
            else {
                interactionText.setIsVisible(false);
            }
        });
}

void MainOverlay::setBuildModeHintVisibility(bool isVisible)
{
    buildModeHintText.setIsVisible(isVisible);
}

AUI::EventResult MainOverlay::onMouseDown(AUI::MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    // Get the first world object under the mouse.
    WorldObjectID objectID{
        worldObjectLocator.getObjectUnderPoint(cursorPosition)};

    // If we hit an entity, pass it to the interaction manager.
    if (entt::entity* entity{std::get_if<entt::entity>(&objectID)}) {
        if (buttonType == AUI::MouseButtonType::Left) {
            interactionManager.entityLeftClicked(*entity);
        }
        else if (buttonType == AUI::MouseButtonType::Right) {
            interactionManager.entityRightClicked(*entity);
        }

        return AUI::EventResult{.wasHandled{true}};
    }

    return AUI::EventResult{.wasHandled{false}};
}

AUI::EventResult
    MainOverlay::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

AUI::EventResult MainOverlay::onMouseMove(const SDL_Point& cursorPosition)
{
    // Get the first world object under the mouse.
    WorldObjectID objectID{
        worldObjectLocator.getObjectUnderPoint(cursorPosition)};

    // If we hit an entity, pass it to InteractionManager.
    entt::entity* entity{std::get_if<entt::entity>(&objectID)};
    if (entity && (*entity != hoveredEntity)) {
        hoveredEntity = *entity;
        interactionManager.entityHovered(*entity);

        return AUI::EventResult{.wasHandled{true}};
    }
    // If we didn't hit an entity, tell InteractionManager to unhover.
    else if (!entity) {
        interactionManager.unhovered();
        hoveredEntity = entt::null;
    }

    return AUI::EventResult{.wasHandled{false}};
}

} // End namespace Client
} // End namespace AM
