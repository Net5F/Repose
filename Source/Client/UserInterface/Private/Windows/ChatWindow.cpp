#include "ChatWindow.h"
#include "Network.h"
#include "Paths.h"
#include "Ignore.h"
#include "AUI/ScalingHelpers.h"
#include <SDL_render.h>
#include <cmath>

namespace AM
{
namespace Client
{
ChatWindow::ChatWindow(Network& inNetwork, SDL_Renderer* inSdlRenderer)
: AUI::Window({6, 528, 642, 216}, "ChatWindow")
, network{inNetwork}
, sdlRenderer{inSdlRenderer}
, alpha{0}
, mouseIsHovering{false}
, shouldFade{false}
, idleTimer{}
, fadeTimer{}
, textureExtent{0, 0, 0, 0}
, renderTexture{nullptr}
, systemMessageQueue{inNetwork.getEventDispatcher()}
, messageContainer({0, 0, logicalExtent.w, logicalExtent.h}, "MessageContainer")
{
    // Add our children so they're included in rendering, etc.
    children.push_back(messageContainer);

    /* Message container. */
    messageContainer.setFlowDirection(
        AUI::VerticalListContainer::FlowDirection::BottomToTop);
}

void ChatWindow::onMouseEnter()
{
    mouseIsHovering = true;
    shouldFade = false;
    setAlpha(255);
}

void ChatWindow::onMouseLeave()
{
    mouseIsHovering = false;
    idleTimer.reset();
}

void ChatWindow::onTick(double timestepS)
{
    // Process any waiting messages.
    SystemMessage systemMessage{};
    while (systemMessageQueue.pop(systemMessage)) {
        // Add the new text.
        // Note: The widget's height will be auto-adjusted to fit the given text.
        std::unique_ptr<AUI::Widget> textPtr{std::make_unique<AUI::Text>(
            SDL_Rect{0, 0, logicalExtent.w, 0},
            "SystemMessageText")};
        AUI::Text& text{static_cast<AUI::Text&>(*textPtr)};
        text.setFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 20);
        text.setColor({255, 255, 255, 255});
        text.setText(systemMessage.messageString);
        text.setAutoHeightEnabled(true);
        text.refreshTexture();

        messageContainer.insert(messageContainer.begin(), std::move(textPtr));

        // If the container has more than our max text widgets, erase the 
        // oldest one.
        if (messageContainer.size() > MAX_MESSAGES) {
            messageContainer.erase(messageContainer.end() - 1);
        }

        idleTimer.reset();
        shouldFade = false;
        setAlpha(255);
    }

    // If we've been idle for long enough, start fading.
    if (!shouldFade && !mouseIsHovering
        && (idleTimer.getTime() > IDLE_TIME_S)) {
        shouldFade = true;
        fadeTimer.reset();
    }

    // If this window is idle and not yet fully faded, step towards it.
    if (shouldFade && (alpha > IDLE_ALPHA)) {
        double t{(FADE_TIME_S - fadeTimer.getTime()) / FADE_TIME_S};
        double newAlpha{std::lerp(IDLE_ALPHA, 255, t)};
        setAlpha(static_cast<Uint8>(newAlpha));
    }
}

void ChatWindow::updateLayout()
{
    // Run the normal layout step.
    Window::updateLayout();

    // If we haven't created our texture or our size has changed, create it.
    if (!renderTexture
        || !((textureExtent.w == scaledExtent.w)
             && (textureExtent.h == scaledExtent.h))) {
        // Create the new texture.
        SDL_Texture* rawTexture{SDL_CreateTexture(
            sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
            scaledExtent.w, scaledExtent.h)};
        if (rawTexture == nullptr) {
            LOG_FATAL("Failed to create texture: %s", SDL_GetError());
        }
        renderTexture = std::shared_ptr<SDL_Texture>(
            rawTexture, [](SDL_Texture* p) { SDL_DestroyTexture(p); });

        textureExtent.w = scaledExtent.w;
        textureExtent.h = scaledExtent.h;

        // Enable alpha transparency.
        SDL_SetTextureBlendMode(renderTexture.get(), SDL_BLENDMODE_BLEND);
    }
}

void ChatWindow::render()
{
    // Note: We render to a separate texture so that we can control the whole 
    //       chat window's transparency.

    // Save the previous renderer state so we can re-apply it after.
    SDL_Texture* previousRenderTarget{SDL_GetRenderTarget(sdlRenderer)};
    SDL_Color previousDrawColor{};
    SDL_GetRenderDrawColor(sdlRenderer, &previousDrawColor.r,
                           &previousDrawColor.g, &previousDrawColor.b,
                           &previousDrawColor.a);

    // Set our texture as the render target and clear it.
    // Note: The draw color must be white even though alpha is 0, because the 
    //       color info is retained and we'll otherwise lose color in blending.
    SDL_SetRenderTarget(sdlRenderer, renderTexture.get());

    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 0);
    SDL_RenderClear(sdlRenderer);
    SDL_SetRenderDrawColor(sdlRenderer, previousDrawColor.r,
                           previousDrawColor.g, previousDrawColor.b,
                           previousDrawColor.a);

    // Render the chat window to our texture.
    // Note: We can't use Window::render() because it passes a screen-relative 
    //       top left and we need it to be window-relative for our texture.
    //       Instead, we rely on Window's clippedExtent which (because of a 
    //       WidgetLocator peculiarity) has its origin at (0, 0).
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.render({clippedExtent.x, clippedExtent.y});
        }
    }

    // Switch back to the previous render target and render our texture.
    SDL_SetRenderTarget(sdlRenderer, previousRenderTarget);
    SDL_RenderCopy(sdlRenderer, renderTexture.get(), &textureExtent,
                   &scaledExtent);
}

void ChatWindow::setAlpha(Uint8 inAlpha)
{
    alpha = inAlpha;
    SDL_SetTextureAlphaMod(renderTexture.get(), alpha);
}

} // End namespace Client
} // End namespace AM
