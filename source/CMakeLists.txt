find_package(Boost REQUIRED COMPONENTS headers)
find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

add_subdirectory(SDLWrap)

# BitBoard

add_library(BitBoard bit_board.cpp)

target_include_directories(BitBoard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitBoard PRIVATE spdlog::spdlog) 

# Chess Pieces

add_library(Pieces pieces.cpp)

target_include_directories(Pieces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Pieces PRIVATE spdlog::spdlog) 
target_link_libraries(Pieces PRIVATE BitBoard)

# SDLApp

add_executable(SDLApp main.cpp grid_view.cpp)

target_link_libraries(SDLApp PRIVATE Boost::headers)
target_link_libraries(SDLApp PRIVATE Microsoft.GSL::GSL)
target_link_libraries(SDLApp PRIVATE spdlog::spdlog) 
target_link_libraries(SDLApp PRIVATE Eigen3::Eigen)
target_link_libraries(SDLApp PRIVATE SDL2::SDL2)
target_link_libraries(SDLApp PRIVATE SDL2_image::SDL2_image)

target_link_libraries(SDLApp PRIVATE SDLWrap)
target_link_libraries(SDLApp PRIVATE SDLImageWrap)
target_link_libraries(SDLApp PRIVATE BitBoard)
target_link_libraries(SDLApp PRIVATE Pieces)
