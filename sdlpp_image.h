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

    [[nodiscard]] generic_error(const generic_error &other) noexcept = default;
    generic_error &operator=(const generic_error &other) noexcept    = default;

    [[nodiscard]] generic_error(generic_error &&other) noexcept = default;
    generic_error &operator=(generic_error &&other) noexcept    = default;

    ~generic_error() noexcept override = default;

    [[nodiscard]] static const char *error()  noexcept
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

class Extensions
{
  public:
    Extensions() = default;
    Extensions(int flags)
    {
        load(flags);
    }

    Extensions(const Extensions &)            = delete;
    Extensions &operator=(const Extensions &) = delete;

    Extensions(Extensions &&)            = delete;
    Extensions &operator=(Extensions &&) = delete;

    ~Extensions()
    {
        IMG_Quit();
    }

    void load(int flags)
    {
        if ((IMG_Init(flags) & flags) == 0) {
            throw exception::initialize_extension{};
        }
    }
};

[[nodiscard]] SurfaceUniquePtr load_image(const std::string &filename);

} // namespace sdl::image
