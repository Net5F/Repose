#include "DialogueWindow.h"
#include "World.h"
#include "Network.h"
#include "Name.h"
#include "DialogueChoiceRequest.h"
#include "Paths.h"
#include "VariantTools.h"
#include "AUI/TextButton.h"

namespace AM
{
namespace Client
{
DialogueWindow::DialogueWindow(World& inWorld, Network& inNetwork)
: AUI::Window({626, 180, 668, 578}, "DialogueWindow")
, world{inWorld}
, network{inNetwork}
, dialogueEvents{}
, choices{}
, processingResponse{false}
, currentTargetEntity{entt::null}
, currentTopicIndex{0}
, waitTimeS{0}
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, dividerImage({4, 408, (logicalExtent.w - 8), 4}, "DividerImage")
, nameText({2, 6, (logicalExtent.w - 4), 44}, "NameText")
, dialogueContainer({12, 60, (logicalExtent.w - 24), 340},
                    "DialogueContainer")
, choiceContainer({12, 422, (logicalExtent.w - 24), 146}, "ChoiceContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(dividerImage);
    children.push_back(nameText);
    children.push_back(dialogueContainer);
    children.push_back(choiceContainer);

    /* Background image. */
    backgroundImage.setNineSliceImage(
        (Paths::TEXTURE_DIR + "TextInput/Normal.png"), {8, 8, 8, 8});
    dividerImage.setSimpleImage(Paths::TEXTURE_DIR + "Window/Divider.png");

    /* Name text. */
    nameText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 26);
    nameText.setColor({255, 255, 255, 255});
    nameText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    nameText.setText("");

    /* Dialogue container. */
    dialogueContainer.setFlowDirection(
        AUI::VerticalListContainer::FlowDirection::BottomToTop);
}

void DialogueWindow::processDialogueResponse(
    const DialogueResponse& dialogueResponse)
{
    // Choice are specific to each response, so we always clear them.
    choices.clear();
    choiceContainer.clear();

    // If the entity ID has changed, clear the dialogue.
    if (dialogueResponse.entity != currentTargetEntity) {
        dialogueEvents = {};
        dialogueContainer.clear();
    }

    currentTargetEntity = dialogueResponse.entity;
    currentTopicIndex = dialogueResponse.topicIndex;

    // Set the target entity's name.
    if (world.registry.valid(currentTargetEntity)) {
        const Name& targetName{world.registry.get<Name>(currentTargetEntity)};
        nameText.setText(targetName.value);
    }
    else {
        nameText.setText("?");
    }

    // Save all the dialogue events and choices. They'll be processed in onTick.
    for (const auto& event : dialogueResponse.dialogueEvents) {
        dialogueEvents.emplace(event);
    }
    for (const auto& choice : dialogueResponse.choices) {
        choices.emplace_back(choice);
    }

    processingResponse = true;
}

void DialogueWindow::clear()
{
    dialogueEvents = {};
    dialogueContainer.clear();
    choices.clear();
    choiceContainer.clear();
}

void DialogueWindow::onTick(double timestepS)
{
    // If we aren't processing a response, return early.
    if (!processingResponse) {
        return;
    }

    // If we're currently waiting, check if enough time has passed.
    if (waitTimeS > 0) {
        waitTimeS -= timestepS;
        if (waitTimeS > 0) {
            // There's still time to wait. Return early.
            return;
        }
    }

    // If there are more dialogue events, process the next one.
    if (!(dialogueEvents.empty())) {
        processDialogueEvent(dialogueEvents.front());
        dialogueEvents.pop();
        return;
    }
    else {
        // We've processed all the events in the response. Add the staged 
        // choices.
        addChoices();
        processingResponse = false;
    }
}

void DialogueWindow::processDialogueEvent(const DialogueEvent& dialogueEvent)
{
    const Name& targetName{world.registry.get<Name>(currentTargetEntity)};
    std::visit(VariantTools::Overload(
                   [&](const SayEvent& sayEvent) {
                       addDialogueText(targetName.value + ": " + sayEvent.text,
                                       {143, 231, 255, 255});
                   },
                   [&](const NarrateEvent& narrateEvent) {
                       addDialogueText(targetName.value + ": "
                                           + narrateEvent.text,
                                       {239, 243, 214, 255});
                   },
                   [&](const WaitEvent& waitEvent) {
                       waitTimeS = waitEvent.waitTimeS;
                   }),
               dialogueEvent);
}

void DialogueWindow::addChoices()
{
    // Add all of the staged choices.
    const Name& playerName{world.registry.get<Name>(world.playerEntity)};
    for (const auto& choice : choices) {
        AUI::TextButton& choiceButton{addChoiceText(choice.displayText)};

        // When pressed, send the choice request and add the selected text 
        // to the dialogue.
        choiceButton.setOnPressed([&]() {
            network.serializeAndSend(DialogueChoiceRequest{
                currentTargetEntity, currentTopicIndex, choice.index});

            addDialogueText(playerName.value + ": " + choice.displayText,
                            {255, 255, 255, 255});
        });
    }
    
    // Add the "End conversation." choice.
    AUI::TextButton& choiceButton{addChoiceText("End conversation.")};
    choiceButton.setOnPressed([&]() { setIsVisible(false); });
}

void DialogueWindow::addDialogueText(std::string_view textString,
                                     const SDL_Color& color)
{
    // Add the new text.
    // Note: The widget's height will be auto-adjusted to fit the given
    // text.
    std::unique_ptr<AUI::Widget> textPtr{std::make_unique<AUI::Text>(
        SDL_Rect{0, 0, (logicalExtent.w - 24), 0}, "DialogueText")};
    AUI::Text& text{static_cast<AUI::Text&>(*textPtr)};
    text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
    text.setColor(color);
    text.setText(textString);
    text.setAutoHeightEnabled(true);
    text.refreshTexture();

    dialogueContainer.insert(dialogueContainer.begin(), std::move(textPtr));

    // Whenever text is added, we want to wait a bit before displaying the 
    // next piece of dialogue.
    // Note: This also delays WaitEvents, meaning any waits are in addition 
    //       to this built-in wait.
    waitTimeS = DIALOGUE_WAIT_TIME_S;
}

AUI::TextButton& DialogueWindow::addChoiceText(std::string_view textString)
{
    // Add the new text button.
    // Note: The widget's height will be auto-adjusted to fit the given
    // text.
    std::unique_ptr<AUI::Widget> textButtonPtr{
        std::make_unique<AUI::TextButton>(
            SDL_Rect{0, 0, (logicalExtent.w - 24), 26}, "DialogueChoice")};
    AUI::TextButton& textButton{static_cast<AUI::TextButton&>(*textButtonPtr)};
    textButton.text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
    textButton.setNormalColor({255, 255, 255, 255});
    textButton.setHoveredColor({210, 210, 210, 255});
    textButton.setPressedColor({168, 168, 168, 255});
    textButton.setDisabledColor({133, 133, 133, 255});
    textButton.setAutoHeightEnabled(true);
    textButton.text.setText(textString);
    textButton.text.setAutoHeightEnabled(true);
    textButton.text.refreshTexture();

    choiceContainer.push_back(std::move(textButtonPtr));

    return textButton;
}

} // End namespace Client
} // End namespace AM
