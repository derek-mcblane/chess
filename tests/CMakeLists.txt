include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
option(INSTALL_GMOCK "" OFF)
option(INSTALL_GTEST "" OFF)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(ChessTest "")
target_compile_features(ChessTest PUBLIC cxx_std_20)
target_compile_options(ChessTest PRIVATE ${CHESS_WARNING_OPTIONS})
target_include_directories(ChessTest PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_sources(ChessTest
PRIVATE
    chess_test.cpp
)
target_link_libraries(ChessTest
PRIVATE
    GTest::gtest_main
    Chess
)
gtest_discover_tests(ChessTest)
