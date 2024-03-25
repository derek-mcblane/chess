# Chess
add_library(Chess "")
target_compile_features(Chess PRIVATE  cxx_std_20)
target_compile_options(Chess PRIVATE ${CHESS_WARNING_OPTIONS})
target_sources(Chess
PUBLIC FILE_SET HEADERS FILES
    pieces.h
    board.h
    game.h
PRIVATE
    pieces.cpp
    board.cpp
    game.cpp
)
target_include_directories(Chess PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(Chess
PUBLIC
    BitBoard
)
install(TARGETS Chess EXPORT ChessTargets
    FILE_SET HEADERS
    INCLUDES DESTINATIION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Chess GUI Application
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
add_executable(ChessApp WIN32 "")
target_sources(ChessApp
PRIVATE FILE_SET HEADERS FILES
    grid_view.h
PRIVATE
    main.cpp
    grid_view.cpp
)
target_compile_features(ChessApp PUBLIC cxx_std_20)
target_compile_options(ChessApp PRIVATE ${CHESS_WARNING_OPTIONS})
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
    SDLWrap::Core
    SDLWrap::Image
    imgui::imgui
    imgui::SDL2Backend
    imgui::SDL2RendererBackend
    BitBoard
    Chess
)

install(TARGETS ChessApp EXPORT ChessTargets)
