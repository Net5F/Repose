#pragma once

#include "AUI/TextInput.h"
#include <string>

namespace AM
{
namespace Client
{
/**
 * The text input style used for the title screen.
 */
class TitleTextInput : public AUI::TextInput
{
public:
    TitleTextInput(const SDL_Rect& inScreenExtent,
                   const std::string& inDebugName = "MainTextInput");
};

} // End namespace Client
} // End namespace AM
