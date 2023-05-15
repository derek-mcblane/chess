add_executable(BitBoardTest "")
target_sources(BitBoardTest PRIVATE bit_board_test.cpp)
target_include_directories(BitBoardTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitBoardTest PRIVATE gtest_main)
target_link_libraries(BitBoardTest PRIVATE BitBoard)
gtest_discover_tests(BitBoardTest)

add_executable(ChessTest "")
target_sources(ChessTest PRIVATE chess_test.cpp)
target_include_directories(ChessTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ChessTest PRIVATE gtest_main)
target_link_libraries(ChessTest PRIVATE Chess)
gtest_discover_tests(ChessTest)

