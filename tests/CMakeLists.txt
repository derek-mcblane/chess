add_executable(BitBoardTest "")
target_sources(BitBoardTest PRIVATE bit_board_test.cpp)
target_include_directories(BitBoardTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BitBoardTest PRIVATE gtest_main)
target_link_libraries(BitBoardTest PRIVATE BitBoard)
gtest_discover_tests(BitBoardTest)

add_executable(PiecesTest "")
target_sources(PiecesTest PRIVATE pieces_test.cpp)
target_include_directories(PiecesTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PiecesTest PRIVATE gtest_main)
target_link_libraries(PiecesTest PRIVATE Pieces)
gtest_discover_tests(PiecesTest)

