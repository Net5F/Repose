#pragma once

#include "AUI/Thumbnail.h"

namespace AM
{
namespace Client
{
/**
 * The thumbnail style used for the main screen.
 */
class BuildModeThumbnail : public AUI::Thumbnail
{
public:
    BuildModeThumbnail(const std::string& inDebugName = "BuildModeThumbnail");
};

} // End namespace Client
} // End namespace AM
