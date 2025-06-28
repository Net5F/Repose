#pragma once

#include "entt/fwd.hpp"
#include "AUI/Window.h"
#include "AUI/Text.h"

namespace AM
{
namespace Client
{
class World;
class WorldObjectLocator;
class Network;
class ViewModel;
class InteractionManager;

/**
 * The main overlay on the main screen. Shows basic HUD information.
 */
class MainOverlay : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    MainOverlay(World& inWorld, const WorldObjectLocator& inWorldObjectLocator,
                Network& inNetwork, ViewModel& inViewModel,
                InteractionManager& inInteractionManager);

    /**
     * Sets the visibility of the hint text.
     * Used by the MainScreen to control the hint text, so we don't have to
     * do "is the player in the build area" checks in two places.
     */
    void setBuildModeHintVisibility(bool isVisible);

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    AUI::EventResult onMouseDown(AUI::MouseButtonType buttonType,
                                 const SDL_Point& cursorPosition) override;

    AUI::EventResult
        onMouseDoubleClick(AUI::MouseButtonType buttonType,
                           const SDL_Point& cursorPosition) override;

    AUI::EventResult onMouseMove(const SDL_Point& cursorPosition) override;

private:
    void onEntityTargeted(entt::entity newTargetedEntity);
    void onInteractionTextUpdated(std::string_view newInteractionText);

    /** Used for getting the world state so we can make decisions and send
        messages. */
    World& world;
    /** Used to see if see if the cursor is hovering or clicking on
        something. */
    const WorldObjectLocator& worldObjectLocator;
    /** Used to send interaction requests. */
    Network& network;
    /** Used to update hovered/targeted entity state. */
    ViewModel& viewModel;
    /** Used to orchestrate item/entity interactions. */
    InteractionManager& interactionManager;

    /** The currently hovered entity. Will be entt::null if no entity is 
        hovered. */
    entt::entity hoveredEntity;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    /** The text at the top of the screen that tells you what your currently 
        selected target is. */
    AUI::Text targetText;

    /** The text at the top of the screen that tells you what action will
        be performed if you click. */
    AUI::Text interactionText;

    /** The "Press 'b' to enter Build Mode" text. */
    AUI::Text buildModeHintText;
};

} // End namespace Client
} // End namespace AM
