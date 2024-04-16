#pragma once

#include "DialogueResponse.h"
#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/VerticalListContainer.h"
#include <SDL_stdinc.h>
#include <queue>

namespace AUI
{
class TextButton;
}

namespace AM
{
namespace Client
{
class World;
class Network;

/**
 * The dialogue window that pops up when you talk to an entity.
 */
class DialogueWindow : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    DialogueWindow(World& inWorld, Network& inNetwork);

    /**
     * Processes the given dialogue response, adding its events to the queue.
     */
    void processDialogueResponse(const DialogueResponse& dialogueResponse);

    /**
     * Clears all dialogue text and choice text widgets from this window.
     */
    void clear();

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

private:
    /**
     * The amount of time to wait between dialogue events.
     * This gives the dialogue a natural pace, without the script needing 
     * a wait between every say command.
     */
    static constexpr float DIALOGUE_WAIT_TIME_S{1.25};

    /**
     * Processes the given dialogue event, adding UI elements or updating 
     * state appropriately.
     */
    void processDialogueEvent(const DialogueEvent& dialogueEvent);

    /**
     * Adds the staged choices to the UI.
     */
    void addChoices();

    /**
     * Adds text to dialogueContainer.
     */
    void addDialogueText(std::string_view textString, const SDL_Color& color);

    /**
     * Adds selectable choice text to choiceContainer.
     */
    AUI::TextButton& addChoiceText(std::string_view textString);

    /** Used to get the target entity's display name. */
    World& world;
    /** Used to send interaction requests. */
    Network& network;

    /** Holds dialogue events that are waiting to be processed. */
    std::queue<DialogueEvent> dialogueEvents;

    /** If we're currently processing dialogue events, this holds the choices 
        that should be presented after all dialogue events are processed. */
    std::vector<DialogueResponse::Choice> choices;

    /** If true, we're currently processing a dialogue response that was 
        received. */
    bool processingResponse;

    /** If processingResponse, this is the entity that is talking (i.e. the 
        entity that was interacted with). */
    entt::entity currentTargetEntity;

    /** If processingResponse, this is the index of the current topic within 
        the entity's Dialogue::topics. */
    Uint8 currentTopicIndex;

    /** If we're actively waiting before processing the next dialogue event, 
        this holds the remaining time to wait in seconds. */
    double waitTimeS;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    AUI::Image backgroundImage;

    /** The divider between the dialogue and choice areas. */
    AUI::Image dividerImage;

    /** The text that shows the target's name at the top of the dialogue. */
    AUI::Text nameText;

    /** Holds the dialogue text. */
    AUI::VerticalListContainer dialogueContainer;

    /** Holds the selectable choice text. */
    AUI::VerticalListContainer choiceContainer;
};

} // End namespace Client
} // End namespace AM
