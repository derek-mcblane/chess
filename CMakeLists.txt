cmake_minimum_required(VERSION 3.15.0)

project(SDL-HelloWorld VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
SET(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

find_package(Boost REQUIRED COMPONENTS headers)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

add_subdirectory(source)

add_executable(SDLApp source/main.cpp source/board.cpp)
target_link_libraries(SDLApp PRIVATE Boost::headers)
target_link_libraries(SDLApp PRIVATE Microsoft.GSL::GSL)
target_link_libraries(SDLApp PRIVATE Eigen3::Eigen)
target_link_libraries(SDLApp PRIVATE SDL2::SDL2)
target_link_libraries(SDLApp PRIVATE SDL2_image::SDL2_image)
target_link_libraries(SDLApp PRIVATE SDLWrap)
target_link_libraries(SDLApp PRIVATE SDLImageWrap)
