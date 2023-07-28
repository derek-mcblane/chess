find_package(Boost 1.80 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

# Chess
add_library(Chess pieces.cpp board.cpp)
target_compile_features(Chess PUBLIC cxx_std_20)
target_include_directories(Chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Chess PUBLIC BitBoard)
target_link_libraries(Chess PRIVATE spdlog::spdlog) 

# Chess GUI Application
add_executable(ChessApp main.cpp grid_view.cpp)
target_compile_features(ChessApp PUBLIC cxx_std_20)
target_link_libraries(ChessApp PRIVATE Boost::headers)
target_link_libraries(ChessApp PRIVATE Microsoft.GSL::GSL)
target_link_libraries(ChessApp PRIVATE spdlog::spdlog) 
target_link_libraries(ChessApp PRIVATE Eigen3::Eigen)
target_link_libraries(ChessApp PRIVATE SDLWrap)
target_link_libraries(ChessApp PRIVATE SDLImageWrap)
target_link_libraries(ChessApp PRIVATE BitBoard)
target_link_libraries(ChessApp PRIVATE Chess)
