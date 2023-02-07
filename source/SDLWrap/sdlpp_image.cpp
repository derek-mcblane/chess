#include "sdlpp_image.h"

#include "sdlpp.h"

namespace sdl::image {

[[nodiscard]] SurfaceUniquePtr load_image(const std::string& filename)
{
    SurfaceUniquePtr image{IMG_Load(filename.c_str())};
    if (image == nullptr) {
        throw exception::load_image{};
    }
    return image;
}

} // namespace sdl::image
