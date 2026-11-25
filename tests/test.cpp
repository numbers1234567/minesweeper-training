
#include "../src/board.h"
//#include "../src/board_clients.h"
#include <iostream>

using namespace MinesweeperLogic;

bool test_reveal() {
    bool mine_map[49]{};
    bool revealed_map[49]{};
    for (int i=16;i<19;i++) mine_map[i]=true;
    mine_map[16+7] = true, mine_map[16+7+2] = true;
    for (int i=16+14;i<19+14;i++) mine_map[i]=true;
    Board board(7, 7, mine_map, revealed_map);

    bool passed = true;
    // Check the is_revealed
    std::cout << "Exposed state" << std::endl;
    std::cout << board.to_str() << std::endl;
    board.reveal(0, 0);
    if (!board.game_state(6,6).is_revealed) passed = false;
    std::cout << "Exposed state after reveal edge" << std::endl;
    std::cout << board.to_str() << std::endl;
    board.reveal(3, 3);
    if (!board.game_state(3,3).is_revealed) passed = false;
    std::cout << "Exposed state after reveal middle" << std::endl;
    std::cout << board.to_str() << std::endl;

    return passed;
}

bool test_factory() {
    DefaultBoardFactory factory(9, 9, 10, false);
    Board board = factory.generate(4, 4);
    std::cout << board.to_str() << std::endl;

    bool passed = true;
    if (!board.reveal(4, 4)) passed = false;
    std::cout << board.to_str() << std::endl;
    factory = DefaultBoardFactory(9, 9, 10, true);
    board = factory.generate(4, 4);
    board.reveal(4, 4);
    if (!board.reveal(4, 4)) passed = false;
    if (board.game_state(4, 4).mine_count > 0) passed = false;
    std::cout << board.to_str() << std::endl;
    return passed;
}

int main() {
    bool test_reveal_res = test_reveal();
    bool test_factory_res = test_factory();
    std::cout << "Reveal test status: " << (test_reveal_res ? "passed" : "failed") << std::endl;
    std::cout << "Factory test status: " << (test_factory_res ? "passed" : "failed") << std::endl;

}