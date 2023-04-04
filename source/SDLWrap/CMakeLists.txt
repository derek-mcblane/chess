find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

add_library(SDLWrap sdlpp.cpp)
target_include_directories(SDLWrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SDLWrap PRIVATE Microsoft.GSL::GSL)
target_link_libraries(SDLWrap PUBLIC SDL2::SDL2)

add_library(SDLImageWrap sdlpp_image.cpp)
target_include_directories(SDLImageWrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SDLImageWrap PRIVATE Microsoft.GSL::GSL)
target_link_libraries(SDLImageWrap PUBLIC SDL2::SDL2)
target_link_libraries(SDLImageWrap PUBLIC SDL2_image::SDL2_image)
