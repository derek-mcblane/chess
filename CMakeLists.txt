cmake_minimum_required(VERSION 3.23.0)

project(Chess VERSION 0.1.0)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(Chess_ENABLE_TESTING "Enable project tests" OFF)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(CHESS_WARNING_OPTIONS
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

find_package(spdlog REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost 1.80 REQUIRED)

add_subdirectory(external EXCLUDE_FROM_ALL SYSTEM)
add_subdirectory(source)

install(DIRECTORY resources DESTINATION bin)

if (${Chess_ENABLE_TESTING})
    enable_testing()
    add_subdirectory(tests)
endif()
