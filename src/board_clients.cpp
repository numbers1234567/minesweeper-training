#include "board_clients.h"
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using namespace MSInterface;
namespace bip=boost::interprocess;

bool BoardWriter::reveal(std::size_t row, std::size_t column) {
    bool result = board->reveal(row, column);
    refresh();
    return result;
}

void SharedMemoryBoardIO::reset_shm(MinesweeperLogic::dim rows, MinesweeperLogic::dim columns) {
    managed_shm_mem = bip::managed_shared_memory(bip::open_only, shm_id.c_str());
    
    mtx_shm_addr = managed_shm_mem.find_or_construct<bip::interprocess_mutex>("mutex")();
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    
    row_shm_addr = managed_shm_mem.find_or_construct<MinesweeperLogic::dim>("rows")(rows);
    col_shm_addr = managed_shm_mem.find_or_construct<MinesweeperLogic::dim>("columns")(columns);
    
    cell_data_shm_addr = managed_shm_mem.find_or_construct<MinesweeperLogic::cell>("cells")[(std::uint16_t)rows*(std::uint16_t)columns]();
    // Some warning here at some point if unset; see the below reset_shm(0, 0) call.
    set = *row_shm_addr > 0 && *col_shm_addr > 0;
}

void SharedMemoryBoardIO::reset_shm() {
    reset_shm(0, 0);
}

void SharedMemoryBoardIO::validate_board() {
    if (!set) throw std::runtime_error("SharedMemoryBoardReader tried to read a garbage board!");
}

SharedMemoryBoardIO::SharedMemoryBoardIO(std::string shm_id, bool priviliged)
: shm_id(shm_id), priviliged(priviliged) {
}

SharedMemoryBoardReader::SharedMemoryBoardReader(std::string shm_id, bool priviliged)
: SharedMemoryBoardIO(shm_id, priviliged) {
    reset_shm();
}

MinesweeperLogic::dim SharedMemoryBoardReader::get_rows() {
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    validate_board();
    
    return *row_shm_addr;
}

MinesweeperLogic::dim SharedMemoryBoardReader::get_columns() {
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    validate_board();
    
    return *col_shm_addr;
}

MinesweeperLogic::cell SharedMemoryBoardReader::get_cell(MinesweeperLogic::dim row, MinesweeperLogic::dim column) {
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    validate_board();
    
    return *(cell_data_shm_addr + row*(*col_shm_addr) + column);
}

BoardData SharedMemoryBoardReader::get_full_state() {
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    validate_board();
    
    BoardData result;
    MinesweeperLogic::dim rows=*row_shm_addr,columns=*col_shm_addr;
    result.rows = rows;
    result.columns = columns;
    auto states = std::make_unique<MinesweeperLogic::cell[]>(rows*columns);
    for (std::uint16_t i;i<rows*columns;i++) {
        states[i] = *(cell_data_shm_addr + i);
    }
    result.states = std::move(states);
    return result;
}


SharedMemoryBoardWriter::SharedMemoryBoardWriter(
    std::shared_ptr<MinesweeperLogic::Board> board, 
    std::string shm_id,  bool priviliged
) : SharedMemoryBoardIO(shm_id, priviliged) {
    this->board = board;
    reset_shm(board->get_rows(), board->get_columns());
    refresh();
}

void SharedMemoryBoardWriter::refresh() {
    auto rows=board->get_rows(), columns=board->get_columns();
    
    // Obtain mutex
    bip::scoped_lock<bip::interprocess_mutex> lock(*mtx_shm_addr);
    // Re-populate
    *row_shm_addr = rows;
    *col_shm_addr = columns;
    for (MinesweeperLogic::dim i=0;i<rows;i++) 
        for (MinesweeperLogic::dim j=0;j<columns;j++) 
            *(cell_data_shm_addr + i*columns + j) = priviliged ? board->state(i, j) : board->game_state(i, j);
}

