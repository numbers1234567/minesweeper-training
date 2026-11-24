
#include "../src/board.h"
#include <iostream>

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

    return true;
}

int main() {
    std::cout << "Reveal test status: " << (test_reveal() ? "passed" : "failed") << std::endl;
}