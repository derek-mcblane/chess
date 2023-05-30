cmake_minimum_required(VERSION 3.15.0)

project(Chess VERSION 0.1.0)

add_subdirectory(external)
add_subdirectory(source)

option(ENABLE_TESTING "Enable project tests" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
