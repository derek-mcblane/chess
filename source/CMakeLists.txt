find_package(Boost REQUIRED COMPONENTS headers)
find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

# Chess
add_library(Chess pieces.cpp board.cpp)
target_compile_features(Chess PUBLIC cxx_std_20)
target_include_directories(Chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Chess PUBLIC BitBoard)
target_link_libraries(Chess PRIVATE spdlog::spdlog) 

# GUI Widgets
add_library(GUIWidgets button.cpp grid_view.cpp)
target_compile_features(GUIWidgets PUBLIC cxx_std_20)
target_include_directories(GUIWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GUIWidgets PRIVATE spdlog::spdlog) 
target_link_libraries(GUIWidgets PRIVATE SDLWrap)
target_link_libraries(GUIWidgets PRIVATE SDLImageWrap)

#Events
add_library(Events INTERFACE)
target_compile_features(Events INTERFACE cxx_std_20)
target_include_directories(Events  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Chess GUI Application
add_executable(ChessApp main.cpp)
target_compile_features(ChessApp PUBLIC cxx_std_20)
target_link_libraries(ChessApp PRIVATE Boost::headers)
target_link_libraries(ChessApp PRIVATE Microsoft.GSL::GSL)
target_link_libraries(ChessApp PRIVATE spdlog::spdlog) 
target_link_libraries(ChessApp PRIVATE Eigen3::Eigen)
target_link_libraries(ChessApp PRIVATE SDLWrap)
target_link_libraries(ChessApp PRIVATE SDLImageWrap)
target_link_libraries(ChessApp PRIVATE BitBoard)
target_link_libraries(ChessApp PRIVATE Chess)
target_link_libraries(ChessApp PRIVATE GUIWidgets)
