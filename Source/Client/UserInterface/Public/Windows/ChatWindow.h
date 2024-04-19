#pragma once

#include "SystemMessage.h"
#include "Timer.h"
#include "AUI/Window.h"
#include "AUI/Text.h"
#include "AUI/VerticalListContainer.h"
#include "QueuedEvents.h"
#include <SDL_stdinc.h>
#include <memory>

struct SDL_Renderer;

namespace AM
{
namespace Client
{
class Network;

/**
 * The chat window on the main screen. Shows player chat and system messages.
 */
class ChatWindow : public AUI::Window
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    ChatWindow(Network& inNetwork, SDL_Renderer* inSdlRenderer);

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    void onMouseEnter() override;

    void onMouseLeave() override;

    void onTick(double timestepS) override;

    void measure() override;

    void render() override;

private:
    /**
     * Sets renderTexture's alpha to the given value.
     */
    void setAlpha(Uint8 inAlpha);

    /** The maximum number of messages that our messageContainer will hold.
        As we receive more, the oldest will be erased. */
    static constexpr std::size_t MAX_MESSAGES{15};

    /** How long this window should stay at full alpha after receiving a
        message or being moused over, before fading to the idle alpha. */
    static constexpr double IDLE_TIME_S{5};

    /** The integer value from 0 - 255 that this window should fade to when
        idle. */
    static constexpr int IDLE_ALPHA{100};

    /** How long it should take to fade from full alpha to IDLE_ALPHA. */
    static constexpr double FADE_TIME_S{0.5};

    /** Used to send interaction requests. */
    Network& network;

    /** Used to render to renderTexture. */
    SDL_Renderer* sdlRenderer;

    /** How opaque from 0 - 255 this window should currently be. */
    Uint8 alpha;

    /** If true, the mouse is currently hovering this window. */
    bool mouseIsHovering;

    /** If true, we should be fading towards IDLE_ALPHA. */
    bool shouldFade;

    /** Tracks how long its been since we've received a message or been hovered
        by the mouse. */
    Timer idleTimer;

    /** Tracks how long we've been fading. */
    Timer fadeTimer;

    /** The texture that we render to, so we can make the chat window
        transparent. */
    std::shared_ptr<SDL_Texture> renderTexture;

    /** The size of renderTexture. */
    SDL_Rect textureExtent;

    EventQueue<SystemMessage> systemMessageQueue;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    /** Holds the player and system messages. */
    AUI::VerticalListContainer messageContainer;
};

} // End namespace Client
} // End namespace AM
