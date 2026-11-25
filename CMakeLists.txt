cmake_minimum_required(VERSION 3.22)
project(MinesweeperTrainer LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS filesystem system )


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tests
    tests/test.cpp
    src/board.cpp
    src/board_clients.cpp
)

add_executable(shm_board_reader
    tests/test_shm_board_reader.cpp
    src/board.cpp
    src/board_clients.cpp
)

target_link_libraries(tests PRIVATE Boost::filesystem Boost::system)
target_link_libraries(shm_board_reader PRIVATE Boost::filesystem Boost::system)