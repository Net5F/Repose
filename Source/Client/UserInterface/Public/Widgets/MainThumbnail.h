#pragma once

#include "AUI/Thumbnail.h"

namespace AM
{
namespace Client
{
/**
 * The thumbnail style used for the main screen.
 */
class MainThumbnail : public AUI::Thumbnail
{
public:
    MainThumbnail(const std::string& inDebugName = "MainThumbnail");
};

} // End namespace Client
} // End namespace AM
