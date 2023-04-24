#pragma once

#include "AUI/Button.h"

namespace AM
{
namespace Client
{
/**
 * The larger button style used for the title screen.
 */
class TitleButton : public AUI::Button
{
public:
    TitleButton(const SDL_Rect& inScreenExtent, const std::string& inText,
                const std::string& inDebugName = "TitleButton");
};

} // End namespace Client
} // End namespace AM
