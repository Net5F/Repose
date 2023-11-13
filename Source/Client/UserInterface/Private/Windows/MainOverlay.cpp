#include "MainOverlay.h"
#include "World.h"
#include "WorldObjectLocator.h"
#include "Network.h"
#include "Name.h"
#include "Interaction.h"
#include "InteractionHelpers.h"
#include "EntityInteractionRequest.h"
#include "Paths.h"
#include "Ignore.h"

namespace AM
{
namespace Client
{
MainOverlay::MainOverlay(World& inWorld,
                         const WorldObjectLocator& inWorldObjectLocator,
                         Network& inNetwork)
: AUI::Window({0, 0, 1920, 1080}, "MainOverlay")
, world{inWorld}
, worldObjectLocator{inWorldObjectLocator}
, network{inNetwork}
, interactionText{{20, 20, 500, 100}, "InteractionText"}
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

    // If we hit an entity with an interaction.
    if (entt::entity* entity{std::get_if<entt::entity>(&objectID)}) {
        if (auto* interaction{world.registry.try_get<Interaction>(*entity)}) {
            if (interaction->isEmpty()) {
                // No interactions, return early.
                return AUI::EventResult{.wasHandled{false}};
            }

            // If the user left-clicked, perform the default interaction.
            if (buttonType == AUI::MouseButtonType::Left) {
                EntityInteractionType defaultInteraction{
                    interaction->supportedInteractions[0]};
                network.serializeAndSend(
                    EntityInteractionRequest{*entity, defaultInteraction});
            }
            else if (buttonType == AUI::MouseButtonType::Right) {
                // TODO: User right-clicked. Open the interaction menu.
            }
        }
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

    // If we hit an entity with an interaction, show it in the interaction text.
    if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
        if (world.registry.all_of<Interaction>(*entity)) {
            auto [name, interaction]
                = world.registry.get<Name, Interaction>(*entity);

            // Count the interactions.
            std::size_t interactionCount{0};
            for (EntityInteractionType interactionType :
                 interaction.supportedInteractions) {
                if (interactionType != EntityInteractionType::NotSet) {
                    interactionCount++;
                }
            }

            if (interactionCount > 0) {
                std::string displayText{InteractionHelpers::toDisplayString(
                    interaction.supportedInteractions[0])};
                displayText += " " + name.value;
                if (interactionCount > 1) {
                    displayText += " / ";
                    displayText += std::to_string(interactionCount);
                    displayText += " more options";
                }

                interactionText.setText(displayText);
                interactionText.setIsVisible(true);

                return AUI::EventResult{.wasHandled{true}};
            }
        }
    }
    else {
        interactionText.setIsVisible(false);
    }

    return AUI::EventResult{.wasHandled{false}};
}

} // End namespace Client
} // End namespace AM
