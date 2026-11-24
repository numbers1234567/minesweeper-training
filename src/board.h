#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct cell {
    std::uint8_t mine_count;
    bool is_revealed;
};

class Board {
private:
    std::vector<std::uint8_t> count_mines();
protected:
    const std::size_t rows;
    const std::size_t columns;
    const std::vector<bool> mine_map;
    std::vector<std::uint8_t> mine_count;
    std::vector<bool> revealed_map;
    std::size_t idx(std::size_t row, std::size_t column);
    std::vector<std::size_t> neighbors(std::size_t idx);
public:
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