find_package(Boost REQUIRED COMPONENTS headers)
find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

# BitBoard
add_library(BitBoard bit_board.cpp)
target_compile_features(BitBoard PUBLIC cxx_std_20)
target_include_directories(BitBoard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitBoard PRIVATE spdlog::spdlog) 

# Chess
add_library(Chess pieces.cpp board.cpp)
target_compile_features(Chess PUBLIC cxx_std_20)
target_include_directories(Chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Chess PRIVATE spdlog::spdlog) 
target_link_libraries(Chess PRIVATE BitBoard)

# GUI Widgets
add_library(GUIWidgets button.cpp grid_view.cpp)
target_compile_features(GUIWidgets PUBLIC cxx_std_20)
target_include_directories(GUIWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GUIWidgets PRIVATE spdlog::spdlog) 
target_link_libraries(GUIWidgets PRIVATE SDLWrap)
target_link_libraries(GUIWidgets PRIVATE SDLImageWrap)

# SDLApp
add_executable(SDLApp main.cpp)
target_compile_features(SDLApp PUBLIC cxx_std_20)
target_link_libraries(SDLApp PRIVATE Boost::headers)
target_link_libraries(SDLApp PRIVATE Microsoft.GSL::GSL)
target_link_libraries(SDLApp PRIVATE spdlog::spdlog) 
target_link_libraries(SDLApp PRIVATE Eigen3::Eigen)
target_link_libraries(SDLApp PRIVATE SDLWrap)
target_link_libraries(SDLApp PRIVATE SDLImageWrap)
target_link_libraries(SDLApp PRIVATE BitBoard)
target_link_libraries(SDLApp PRIVATE Chess)
target_link_libraries(SDLApp PRIVATE GUIWidgets)
