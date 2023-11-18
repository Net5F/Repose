#include "BuildModeThumbnail.h"
#include "Paths.h"

namespace AM
{
namespace Client
{
BuildModeThumbnail::BuildModeThumbnail(const std::string& inDebugName)
: AUI::Thumbnail({0, 0, 108, 109}, inDebugName)
{
    // Add our backgrounds.
    hoveredImage.setSimpleImage(Paths::TEXTURE_DIR + "Thumbnail/Hovered.png");
    activeImage.setSimpleImage(Paths::TEXTURE_DIR + "Thumbnail/Active.png");
    backdropImage.setSimpleImage(Paths::TEXTURE_DIR + "Thumbnail/Backdrop.png");
    selectedImage.setSimpleImage(Paths::TEXTURE_DIR + "Thumbnail/Selected.png");

    // Move our thumbnail image to the right position.
    thumbnailImage.setLogicalExtent({6, 4, 96, 96});

    // Set our text properties.
    setTextLogicalExtent({0, 0, 1, 1});
    setTextFont((Paths::FONT_DIR + "Cagliostro-Regular.ttf"), 15);
    setTextColor({255, 255, 255, 255});
}

} // End namespace Client
} // End namespace AM
