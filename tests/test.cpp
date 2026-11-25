
#include "../src/board.h"
#include "../src/board_clients.h"
#include <boost/process.hpp>
#include <iostream>

using namespace MinesweeperLogic;
using namespace MSInterface;
namespace bp = boost::process;
namespace bip = boost::interprocess;

Board board_8() {
    bool mine_map[49]{};
    bool revealed_map[49]{};
    for (int i=16;i<19;i++) mine_map[i]=true;
    mine_map[16+7] = true, mine_map[16+7+2] = true;
    for (int i=16+14;i<19+14;i++) mine_map[i]=true;
    Board board(7, 7, mine_map, revealed_map);
    return board;
}

bool test_reveal() {
    Board board = board_8();
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

bool test_shm() {
    bool passed=true;

    struct shm_remove
    {
        shm_remove() {
            bip::shared_memory_object::remove("test1");
            bip::shared_memory_object::remove("test2");
        }
        ~shm_remove(){
            bip::shared_memory_object::remove("test1");
            bip::shared_memory_object::remove("test2");
        }
    } remover;
    bip::managed_shared_memory shm1(bip::create_only, "test1", 65536);
    bip::managed_shared_memory shm2(bip::create_only, "test2", 65536);
    {
    bp::opstream into;
    bp::child c("shm_board_reader", "test1", "priviliged", bp::std_in < into);
    std::cout << "Init child" << std::endl;
    sleep(1);
    into << "go" << std::endl;
    into << "end" << std::endl;
    
    c.wait();
    if (c.exit_code() == 0) passed = false;
    }
    {
    Board board = board_8();
    std::shared_ptr<Board> ptr = std::make_shared<Board>(board);
    MSInterface::SharedMemoryBoardWriter writer(ptr, "test2", false);

    bp::opstream into;
    bp::ipstream out;

    bp::child c("shm_board_reader", "test2", "unpriviliged", bp::std_in < into, bp::std_out > out);
    into << "go" << std::endl;
    std::string res;
    std::getline(out, res);
    std::cout << "dim " << res << std::endl;
    for (int i=0;i<7;i++) {
        std::getline(out, res);
        if (res != "#######") passed = false;
        std::cout << res << std::endl;
    }
    std::cout << "Revealing" << std::endl;
    writer.reveal(0, 0);
    std::cout << "Revealed" << std::endl;
    into << "go" << std::endl;
    std::getline(out, res);
    std::getline(out, res);
    std::cout << res << std::endl;
    if (res != "0000000") passed = false;
    into << "end" << std::endl;
    c.wait();
    if (c.exit_code() != 0) passed=false;
    }

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
    bool test_shm_res = test_shm();
    std::cout << "Reveal test status: " << (test_reveal_res ? "passed" : "failed") << std::endl;
    std::cout << "Factory test status: " << (test_factory_res ? "passed" : "failed") << std::endl;
    std::cout << "Shared memory test status: " << (test_shm_res ? "passed" : "failed") << std::endl;

}