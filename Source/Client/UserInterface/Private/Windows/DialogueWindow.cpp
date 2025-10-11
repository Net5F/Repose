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
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h}, "BackgroundImage")
, dividerImage({4, 408, (logicalExtent.w - 8), 4}, "DividerImage")
, nameText({2, 6, (logicalExtent.w - 4), 44}, "NameText")
, proceedText({12, 422, (logicalExtent.w - 24), 146}, "ProceedText")
, dialogueContainer({12, 60, (logicalExtent.w - 24), 340},
                    "DialogueContainer")
, choiceContainer({12, 422, (logicalExtent.w - 24), 146}, "ChoiceContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(dividerImage);
    children.push_back(nameText);
    children.push_back(proceedText);
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

    /* Proceed text. */
    proceedText.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
    proceedText.setColor({255, 255, 255, 255});
    proceedText.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);
    proceedText.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
    proceedText.setText("Click to proceed.");

    /* Dialogue container. */
    dialogueContainer.setFlowDirection(
        AUI::VerticalListContainer::FlowDirection::BottomToTop);
}

void DialogueWindow::processDialogueResponse(
    const DialogueResponse& dialogueResponse)
{
    // Choices are specific to each response, so we always clear them.
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

    // Stage the choices.
    for (const auto& choice : dialogueResponse.choices) {
        choices.emplace_back(choice);
    }

    // If there's no dialogue, display the choices.
    if (dialogueResponse.dialogueEvents.empty()) {
        addChoices();
    }
    // If there's one dialogue event, display it and the choices.
    else if (dialogueResponse.dialogueEvents.size() == 1) {
        processDialogueEvent(dialogueResponse.dialogueEvents.front());
        addChoices();
    }
    // There are multiple dialogue events. Display the first and save the rest.
    else {
        processDialogueEvent(dialogueResponse.dialogueEvents.front());

        for (std::size_t i{1}; i < dialogueResponse.dialogueEvents.size();
             ++i) {
            dialogueEvents.emplace(dialogueResponse.dialogueEvents.at(i));
        }

        proceedText.setIsVisible(true);
        processingResponse = true;
    }
}

void DialogueWindow::clear()
{
    dialogueEvents = {};
    dialogueContainer.clear();
    choices.clear();
    choiceContainer.clear();
}

AUI::EventResult DialogueWindow::onMouseDown(AUI::MouseButtonType buttonType,
                                             const SDL_Point& cursorPosition)
{
    // Only respond to the left mouse button.
    if (buttonType != AUI::MouseButtonType::Left) {
        return AUI::EventResult{.wasHandled{false}};
    }
    // If the choice area wasn't hit or we aren't processing a response, return
    // early.
    else if (!SDL_PointInRect(&cursorPosition, &(choiceContainer.getClippedExtent()))
        || !processingResponse) {
        return AUI::EventResult{.wasHandled{false}};
    }

    // If there are more dialogue events, process the next one.
    if (!(dialogueEvents.empty())) {
        processDialogueEvent(dialogueEvents.front());
        dialogueEvents.pop();
        proceedText.setIsVisible(true);

        // If we've processed all the events in the response, add the staged 
        // choices.
        if (dialogueEvents.empty()) {
            addChoices();
            processingResponse = false;
        }
    }

    return AUI::EventResult{.wasHandled{true}};
}

AUI::EventResult
    DialogueWindow::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

void DialogueWindow::processDialogueEvent(const DialogueEvent& dialogueEvent)
{
    std::visit(VariantTools::Overload(
                   [&](const SayEvent& sayEvent) {
                       addDialogueText(sayEvent.text, {143, 231, 255, 255});
                   },
                   [&](const NarrateEvent& narrateEvent) {
                       addDialogueText(narrateEvent.text, {239, 243, 214, 255});
                   }),
               dialogueEvent);
}

void DialogueWindow::addChoices()
{
    // Add all of the staged choices.
    for (const auto& choice : choices) {
        AUI::TextButton& choiceButton{addChoiceText(choice.displayText)};

        // When pressed, send the choice request and add the selected text 
        // to the dialogue.
        choiceButton.setOnPressed([&]() {
            network.serializeAndSend(DialogueChoiceRequest{
                currentTargetEntity, currentTopicIndex, choice.index});

            addDialogueText("> " + choice.displayText, {255, 255, 255, 255});
        });
    }
    
    // Add the "End conversation." choice.
    AUI::TextButton& choiceButton{addChoiceText("End conversation.")};
    choiceButton.setOnPressed([&]() { setIsVisible(false); });

    // Hide the "click to proceed" text.
    proceedText.setIsVisible(false);
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
