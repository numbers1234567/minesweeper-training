#include "board.h"
#include <queue>
#include <iostream>

std::size_t Board::ext2int_idx(std::size_t row, std::size_t column) { return row*rows + column; }

Board::Board(std::size_t rows, std::size_t columns, bool mine_map[], bool revealed_map[]) 
    : rows(rows), columns(columns), mine_map(std::vector<bool>(mine_map, mine_map + rows*columns)) {
    this->revealed_map = std::vector<bool>(rows*columns, false);

    // Pre-count mines
    for (std::size_t i=0;i<rows*columns;i++) {
        std::uint8_t count=0;
        auto neighbors = this->neighbors(i);
        for (auto it=neighbors.begin();it!=neighbors.end();++it) count += mine_map[*it];
        mine_count.push_back(count);
    }
    // Reveal cells
    for (std::size_t i=0;i<rows;i++)
        for (std::size_t j=0;j<columns;j++) 
            if (revealed_map[ext2int_idx(i,j)]) reveal(i,j);
}

std::vector<std::size_t> Board::neighbors(std::size_t idx) {
    std::size_t cell_row = idx / rows;
    std::size_t cell_col = idx % columns;
    std::size_t row_lower = std::max((int)cell_row-1, 0), row_upper = std::min(cell_row+2, rows);
    std::size_t col_lower = std::max((int)cell_col-1, 0), col_upper = std::min(cell_col+2, columns);
    std::vector<std::size_t> ret;
    for (std::size_t next_row=row_lower;next_row<row_upper;next_row++)
        for (std::size_t next_col=col_lower;next_col<col_upper;next_col++) {
            ret.push_back(this->ext2int_idx(next_row, next_col));
        }
    return ret;
}

cell Board::game_state(std::size_t row, std::size_t column) {
    cell result;
    auto mapped = ext2int_idx(row, column);
    result.is_revealed = revealed_map[mapped];
    if (!result.is_revealed) return result;
    result.mine_count = mine_count[mapped];
    return result;
}

cell Board::state(std::size_t row, std::size_t column) {
    cell result;
    auto mapped = ext2int_idx(row, column);
    result.is_revealed = revealed_map[mapped];
    result.mine_count = mine_count[mapped];
    return result;
}

std::string Board::to_str() {
    char result[rows*columns + rows];
    std::size_t char_idx = 0;
    for (std::size_t row=0;row<rows;row++) {
        for (std::size_t col=0;col<columns;col++) {
            cell state = game_state(row,col);
            if (state.is_revealed) result[char_idx++] = '0' + state.mine_count;
            else                   result[char_idx++] = 'x';
        }
        result[char_idx++] = '\n';
    }
    return std::string(result, result+char_idx);
}

bool Board::reveal(std::size_t row, std::size_t column) {
    std::size_t mapped_idx = ext2int_idx(row, column);
    if (mine_map[mapped_idx]) return false;
    if (revealed_map[mapped_idx]) return true;
    // Reveal this cell, automatically revealing neighbors of revealed cells with 0 neighboring mines.
    std::queue<std::size_t> q;
    q.push(mapped_idx);
    while (!q.empty()) {
        std::size_t cell_idx = q.front();
        q.pop();
        if (revealed_map[cell_idx]) continue;
        revealed_map[cell_idx] = true;
        if (mine_count[cell_idx] > 0) continue;

        auto neighbors = this->neighbors(cell_idx);
        for (auto it=neighbors.begin();it!=neighbors.end();++it) 
            q.push(*it);
    }
    return true;
}

DefaultBoardFactory::DefaultBoardFactory(
    std::size_t rows, std::size_t columns, std::size_t mine_count, bool guarantee_0_start
) : rows(rows), columns(columns), mine_count(mine_count), guarantee_0_start(guarantee_0_start) {
    if (mine_count + 1 + 8*guarantee_0_start > rows*columns) throw std::invalid_argument("Too many mines for this board setting");
    
    std::random_device rd;
    gen = std::mt19937(rd());
    dist = std::uniform_int_distribution<int>(0, rows*columns-1);

    bool dummy_mine_map[rows*columns]{};
    bool dummy_reveal_map[rows*columns]{};

    dummy_board.emplace(Board(rows, columns, dummy_mine_map, dummy_reveal_map));
}

Board DefaultBoardFactory::generate() {
    std::size_t left=mine_count;

    bool revealed[rows*columns]{};
    bool mines[rows*columns]{};

    while (left > 0) {
        std::size_t place_idx = (std::size_t)dist(gen);
        if (mines[place_idx]) continue;
        mines[place_idx] = true;
        left--;
    }
    return Board(rows, columns, mines, revealed);
}
Board DefaultBoardFactory::generate(std::size_t start_row, std::size_t start_col) {
    std::size_t left=mine_count;

    bool revealed[rows*columns]{};
    bool mines[rows*columns]{};

    revealed[start_row*columns + start_col] = true;

    while (left > 0) {
        std::size_t place_idx = (std::size_t)dist(gen);
        if (mines[place_idx]) continue;
        if (revealed[place_idx]) continue;
        if (guarantee_0_start) {
            auto neighbors = (*dummy_board).neighbors(place_idx);
            bool works=true;
            for (auto it=neighbors.begin();it != neighbors.end(); ++it)
                if (revealed[*it]) works = false;
            if (!works) continue;
        }
        mines[place_idx] = true;
        left--;
    }
    return Board(rows, columns, mines, revealed);
}