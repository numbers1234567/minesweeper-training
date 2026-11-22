#pragma once

#include <vector>

struct cell {
    bool has_mine;
    bool is_revealed;
};

class Board {
protected:
    int rows;
    int columns;
    std::vector<bool> mine_map;
    std::vector<bool> revealed_map;
public:
    Board(
        int rows,
        int columns,
        bool mine_map[],
        bool revealed_map[]
    );
    bool Board::reveal(int row, int column);
    cell Board::game_state(int row, int column);
    cell Board::state(int row, int column);
};