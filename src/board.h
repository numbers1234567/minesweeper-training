#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <random>
#include <optional>
#include <tuple>

namespace MinesweeperLogic {

typedef std::uint8_t dim;

struct cell {
    std::uint8_t mine_count;
    bool has_mine;
    bool is_revealed;
};

class Board {
private:
    std::vector<std::uint8_t> count_mines();
protected:
    dim rows;
    dim columns;
    std::vector<bool> mine_map;
    std::vector<std::uint8_t> mine_count;
    std::vector<bool> revealed_map;
public:
    std::size_t ext2int_idx(dim row, dim column);
    std::tuple<dim, dim> int2ext_idx(std::size_t idx);
    std::vector<std::size_t> neighbors(std::size_t idx);
    Board(
        dim rows,
        dim columns,
        bool mine_map[],
        bool revealed_map[]
    );
    bool reveal(dim row, dim column);
    cell game_state(dim row, dim column);
    cell state(dim row, dim column);

    dim get_rows();
    dim get_columns();

    std::string to_str();
};

// Anticipate different variants because of the nature of this project
class BoardFactory {
    public:
    virtual Board generate() =0;
    virtual Board generate(dim start_row, dim start_col) =0;

    virtual ~BoardFactory() = default;
};

// Default algorithm that probably almost all Minesweeper implementations use
class DefaultBoardFactory : public BoardFactory {
private:
    dim rows, columns;
    std::uint16_t mine_count;
    bool guarantee_0_start;

    std::uniform_int_distribution<int> dist;
    std::mt19937 gen;

    std::optional<Board> dummy_board;
public:
    DefaultBoardFactory(
        dim rows, dim columns, std::uint16_t mine_count,
        bool guarantee_0_start
    );
    Board generate();
    Board generate(dim start_row, dim start_col);
};
}