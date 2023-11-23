add_subdirectory(SDLWrap)
add_subdirectory(bit-board)

option(IMGUI_DEFINE_MATH_OPERATORS "" ON)
option(IMGUI_USE_STDLIB_SUPPORT "" ON)
add_subdirectory(imgui SYSTEM)
