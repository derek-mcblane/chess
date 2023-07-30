add_subdirectory(SDLWrap)
add_subdirectory(bit-board)

add_library(imgui)
target_compile_features(imgui PUBLIC cxx_std_20)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui)

file(GLOB IMGUI_HEADERS CONFIGURE_DEPENDS imgui/*.h)
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS imgui/*.cpp)
option(IMGUI_USE_STDLIB_SUPPORT "" ON)
if(IMGUI_USE_STDLIB_SUPPORT)
    list(APPEND IMGUI_HEADERS imgui/misc/cpp/imgui_stdlib.h)
    list(APPEND IMGUI_SOURCES imgui/misc/cpp/imgui_stdlib.cpp)
endif()
target_sources(imgui PUBLIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})


add_library(imgui_SDL2Backend
    imgui/backends/imgui_impl_sdl2.h
    imgui/backends/imgui_impl_sdl2.cpp
)
target_compile_features(imgui_SDL2Backend PUBLIC cxx_std_20)
target_include_directories(imgui_SDL2Backend PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui)
target_link_libraries(imgui_SDL2Backend PRIVATE SDLWrap)


add_library(imgui_SDL2RendererBackend
    imgui/backends/imgui_impl_sdlrenderer2.h
    imgui/backends/imgui_impl_sdlrenderer2.cpp
)
target_compile_features(imgui_SDL2RendererBackend PUBLIC cxx_std_20)
target_include_directories(imgui_SDL2RendererBackend PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui)
target_link_libraries(imgui_SDL2RendererBackend PRIVATE SDLWrap)
