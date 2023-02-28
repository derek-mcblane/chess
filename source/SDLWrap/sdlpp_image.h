#pragma once

#include "sdlpp.h"

#include "SDL_image.h"

#include <stdexcept>

namespace sdl::image {

namespace exception {

class generic_error : virtual public std::runtime_error
{
  public:
    [[nodiscard]] generic_error() : std::runtime_error(IMG_GetError()) {}

    [[nodiscard]] generic_error(const generic_error& other) noexcept = default;
    generic_error& operator=(const generic_error& other) noexcept = default;

    [[nodiscard]] generic_error(generic_error&& other) noexcept = default;
    generic_error& operator=(generic_error&& other) noexcept = default;

    ~generic_error() noexcept override = default;

    [[nodiscard]] static const char* error() noexcept
    {
        return IMG_GetError();
    }
};

class initialize_extension : virtual public std::runtime_error
{
  public:
    [[nodiscard]] initialize_extension() : std::runtime_error(IMG_GetError()) {}
};

class load_image : virtual public std::runtime_error
{
  public:
    [[nodiscard]] load_image() : std::runtime_error(IMG_GetError()) {}
};

} // namespace exception

enum class InitFlags
{
    jpg = 0x00000001U,
    png = 0x00000002U,
    tif = 0x00000004U,
    webp = 0x00000008U,
    jxl = 0x00000010U,
    avif = 0x00000020U,
};

inline void initialize(int flags)
{
    if ((IMG_Init(flags) & flags) == 0) {
        throw exception::initialize_extension{};
    }
}

inline void initialize(InitFlags flags)
{
    initialize(static_cast<int>(flags));
}

inline void quit()
{
    IMG_Quit();
}

class Context
{
  public:
    Context(int flags)
    {
        initialize(flags);
    }

    Context(InitFlags flags)
    {
        initialize(flags);
    }

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    ~Context()
    {
        quit();
    }
};

[[nodiscard]] SurfaceUniquePtr load_image(const std::string& filename);

} // namespace sdl::image
