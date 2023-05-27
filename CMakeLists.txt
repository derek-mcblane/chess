cmake_minimum_required(VERSION 3.15.0)

project(Chess VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall)

add_subdirectory(external)
add_subdirectory(source)

option(ENABLE_TESTING "Enable project tests" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
