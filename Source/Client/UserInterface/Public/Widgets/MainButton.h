#pragma once

#include "AUI/Screen.h"
#include "AUI/Button.h"
#include <string>

namespace AM
{
class AssetCache;

namespace Client
{
/**
 * The regular-sized, general-purpose button style.
 */
class MainButton : public AUI::Button
{
public:
    MainButton(AssetCache& assetCache, const SDL_Rect& inScreenExtent,
               const std::string& inText,
               const std::string& inDebugName = "MainButton");
};

} // End namespace Client
} // End namespace AM
