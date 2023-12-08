add_subdirectory(SDLWrap EXCLUDE_FROM_ALL)
add_subdirectory(bit-board)

option(IMGUI_DEFINE_MATH_OPERATORS "" ON)
option(IMGUI_USE_STDLIB_SUPPORT "" ON)

add_library(ImguiUserConfig INTERFACE)
target_include_directories(ImguiUserConfig INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ImguiUserConfig INTERFACE SDLWrap::Core)
set(IMGUI_USER_CONFIG_TARGET ImguiUserConfig CACHE STRING "")
set(IMGUI_USER_CONFIG imgui_chess_config.h CACHE STRING "")
add_subdirectory(imgui SYSTEM)
