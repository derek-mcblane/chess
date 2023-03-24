add_executable(RunUnitTests "")
target_sources(RunUnitTests PRIVATE bit_board_test.cpp)
target_include_directories(RunUnitTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RunUnitTests PRIVATE gtest_main)
target_link_libraries(RunUnitTests PRIVATE BitBoard)

gtest_discover_tests(RunUnitTests)

