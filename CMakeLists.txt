cmake_minimum_required(VERSION 3.22)
project(MinesweeperTrainer LANGUAGES CXX)

#find_package(Boost REQUIRED COMPONENTS filesystem system)
#target_link_libraries(your_target PRIVATE Boost::filesystem Boost::system)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tests
    tests/test.cpp
    src/board.cpp
)