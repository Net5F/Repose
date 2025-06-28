#include "MainOverlay.h"
#include "World.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "ViewModel.h"
#include "InteractionManager.h"
#include "Name.h"
#include "Paths.h"

namespace AM
{
namespace Client
{
MainOverlay::MainOverlay(World& inWorld,
                         const WorldObjectLocator& inWorldObjectLocator,
                         Network& inNetwork, ViewModel& inViewModel,
                         InteractionManager& inInteractionManager)
: AUI::Window({0, 0, 1920, 1080}, "MainOverlay")
, world{inWorld}
, worldObjectLocator{inWorldObjectLocator}
, network{inNetwork}
, viewModel{inViewModel}
, interactionManager{inInteractionManager}
, hoveredEntity{entt::null}
, targetText{{0, 20, logicalExtent.w, logicalExtent.h}, "TargetText"}
, interactionText{{20, 20, 700, 100}, "InteractionText"}
, buildModeHintText({50, 850, 500, 500}, "BuildModeHintText")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(targetText);
    children.push_back(interactionText);
    children.push_back(buildModeHintText);

    /* Target text. */
    targetText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    targetText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    targetText.setColor({255, 255, 255, 255});
    targetText.setIsVisible(false);

    /* Interaction text. */
    interactionText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    interactionText.setColor({255, 255, 255, 255});
    interactionText.setIsVisible(false);

    /* Build mode text. */
    buildModeHintText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 30);
    buildModeHintText.setColor({255, 255, 255, 255});
    buildModeHintText.setText("Press 'b' to enter Build Mode.");
    buildModeHintText.setIsVisible(false);

    // When an entity is targeted, update targetText.
    viewModel.entityTargeted.connect<&MainOverlay::onEntityTargeted>(this);
    // Note: We don't subscribe to entityHovered because we're the only 
    //       source of entity hover events.
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
    if (entt::entity* entity{std::get_if<entt::entity>(&objectID)};
        entity && world.registry.valid(*entity)) {
        // Set the entity as our current target.
        viewModel.setTargetEntity(*entity);

        if (buttonType == AUI::MouseButtonType::Left) {
            interactionManager.entityLeftClicked(*entity);
        }
        else if (buttonType == AUI::MouseButtonType::Right) {
            interactionManager.entityRightClicked(*entity);
        }

        return AUI::EventResult{.wasHandled{true}};
    }
    else {
        // Not an entity. Clear the target selection.
        viewModel.setTargetEntity(entt::null);
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

    // If we hit an entity, update the model.
    entt::entity* entity{std::get_if<entt::entity>(&objectID)};
    if (entity && world.registry.valid(*entity) && (*entity != hoveredEntity)) {
        const Name& name{world.registry.get<Name>(*entity)};
        viewModel.setHoveredEntity(name.value);
        hoveredEntity = *entity;

        return AUI::EventResult{.wasHandled{true}};
    }
    // If we didn't hit an entity, tell InteractionManager to unhover.
    else if (!entity) {
        viewModel.clearHoveredEntity();
        hoveredEntity = entt::null;
    }

    return AUI::EventResult{.wasHandled{false}};
}

void MainOverlay::onEntityTargeted(entt::entity newTargetedEntity)
{
    // Note: We need to do this here instead of onMouseDown because we may not 
    //       be the only source of entity target changes.
    if (newTargetedEntity != entt::null) {
        const Name& name{world.registry.get<Name>(newTargetedEntity)};
        targetText.setText(name.value);
        targetText.setIsVisible(true);
    }
    else {
        targetText.setIsVisible(false);
    }
}

void MainOverlay::onInteractionTextUpdated(std::string_view newInteractionText)
{
    if (newInteractionText != "") {
        interactionText.setText(newInteractionText);
        interactionText.setIsVisible(true);
    }
    else {
        interactionText.setIsVisible(false);
    }
}

} // End namespace Client
} // End namespace AM
