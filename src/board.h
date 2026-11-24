#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <random>
#include <optional>

struct cell {
    std::uint8_t mine_count;
    bool is_revealed;
};

class Board {
private:
    std::vector<std::uint8_t> count_mines();
protected:
    std::size_t rows;
    std::size_t columns;
    std::vector<bool> mine_map;
    std::vector<std::uint8_t> mine_count;
    std::vector<bool> revealed_map;
public:
    std::size_t ext2int_idx(std::size_t row, std::size_t column);
    std::size_t int2ext_idx(std::size_t idx);
    std::vector<std::size_t> neighbors(std::size_t idx);
    Board(
        std::size_t rows,
        std::size_t columns,
        bool mine_map[],
        bool revealed_map[]
    );
    bool reveal(std::size_t row, std::size_t column);
    cell game_state(std::size_t row, std::size_t column);
    cell state(std::size_t row, std::size_t column);

    std::string to_str();
};

// Anticipate different variants because of the nature of this project
class BoardFactory {
    public:
    virtual Board generate() =0;
    virtual Board generate(std::size_t start_row, std::size_t start_col) =0;

    virtual ~BoardFactory() = default;
};

// Default algorithm that probably almost all Minesweeper implementations use
class DefaultBoardFactory : public BoardFactory {
private:
    std::size_t rows, columns, mine_count;
    bool guarantee_0_start;

    std::uniform_int_distribution<int> dist;
    std::mt19937 gen;

    std::optional<Board> dummy_board;
public:
    DefaultBoardFactory(
        std::size_t rows, std::size_t columns, std::size_t mine_count,
        bool guarantee_0_start
    );
    Board generate();
    Board generate(std::size_t start_row, std::size_t start_col);
};