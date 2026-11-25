/*
Define interface to game logic

A reader can be priviliged or unpriviliged (or admin vs player view, essentially)
*/
#include "board.h"
#include <memory>
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace MSInterface {

struct BoardData {
    MinesweeperLogic::dim rows,columns;
    std::unique_ptr<MinesweeperLogic::cell[]> states;
};

class BoardReader {
public:
    virtual MinesweeperLogic::dim get_rows() =0;
    virtual MinesweeperLogic::dim get_columns() =0;
    virtual MinesweeperLogic::cell get_cell(MinesweeperLogic::dim row, MinesweeperLogic::dim column) =0;
    virtual BoardData get_full_state() =0;
};

class BoardWriter {
protected:
    // Assume it has access to the board
    std::shared_ptr<MinesweeperLogic::Board> board;
public:
    virtual void refresh() =0;
    bool reveal(std::size_t row, std::size_t column);
};

class SharedMemoryBoardIO {
    // Might store board deltas in the future, but it's not clear if there would be a tangible benefit.

    protected:
    boost::interprocess::managed_shared_memory managed_shm_mem;
    boost::interprocess::interprocess_mutex *mtx_shm_addr;
    MinesweeperLogic::dim *row_shm_addr, *col_shm_addr;
    MinesweeperLogic::cell *cell_data_shm_addr;
    
    // Throws an exception if the stored board breaks a constraint, or if we try to access data when set=false.
    void validate_board();
    // Whether memory is populated with a valid board.
    bool set;
    void reset_shm();
    void reset_shm(MinesweeperLogic::dim rows, MinesweeperLogic::dim columns);
public:
    SharedMemoryBoardIO(std::string shm_id, bool priviliged);
    const bool priviliged;
    const std::string shm_id;
};

class SharedMemoryBoardReader : public BoardReader, public SharedMemoryBoardIO {
public:
    SharedMemoryBoardReader(std::string shm_id, bool priviliged);
    MinesweeperLogic::dim get_rows();
    MinesweeperLogic::dim get_columns();
    MinesweeperLogic::cell get_cell(MinesweeperLogic::dim row, MinesweeperLogic::dim column);
    BoardData get_full_state();
};

class SharedMemoryBoardWriter : public BoardWriter, public SharedMemoryBoardIO{
public:

    SharedMemoryBoardWriter(std::shared_ptr<MinesweeperLogic::Board> board, std::string shm_id, bool priviliged);
    void refresh();
};

}