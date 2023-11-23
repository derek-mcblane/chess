find_package(Boost 1.80 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

# Chess
add_library(Chess pieces.cpp board.cpp game.cpp)
target_compile_features(Chess PUBLIC cxx_std_20)
target_compile_options(Chess PUBLIC ${CHESS_WARNING_OPTIONS})
target_include_directories(Chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Chess PUBLIC BitBoard)
#target_link_libraries(Chess PRIVATE spdlog::spdlog) 

# Chess GUI Application
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
add_executable(ChessApp WIN32 main.cpp grid_view.cpp)
target_compile_features(ChessApp PUBLIC cxx_std_20)
target_compile_options(ChessApp PUBLIC ${CHESS_WARNING_OPTIONS})
target_link_libraries(ChessApp
PRIVATE
    Microsoft.GSL::GSL
    spdlog::spdlog
    Boost::headers
)
if(TARGET SDL2::SDL2main)
    target_link_libraries(ChessApp PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(ChessApp
PRIVATE
    SDLWrap
    SDLImageWrap
    imgui::imgui
    imgui::SDL2Backend
    imgui::SDL2RendererBackend
    BitBoard
    Chess
)

install(TARGETS ChessApp 
    CONFIGURATIONS Debug Release
    RUNTIME DESTINATION bin
)
