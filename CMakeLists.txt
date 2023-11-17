cmake_minimum_required(VERSION 3.21.0)

project(Chess VERSION 0.1.0)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(CHESS_WARNING_OPTIONS
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

add_subdirectory(external)
add_subdirectory(source)

install(DIRECTORY resources DESTINATION bin)

option(ENABLE_TESTING "Enable project tests" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
