cmake_minimum_required(VERSION 3.21.0)

project(Chess VERSION 0.1.0)

add_subdirectory(external)
add_subdirectory(source)

install(DIRECTORY resources DESTINATION bin)

option(ENABLE_TESTING "Enable project tests" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
