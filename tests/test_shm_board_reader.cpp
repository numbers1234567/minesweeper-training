#include "../src/board_clients.h"
#include <iostream>


int main(int argc, char *argv[]) {
    if (argc < 3) throw std::invalid_argument("Not enough args");
    std::string shm_id(argv[1]);
    std::string priv_str(argv[2]);
    bool priviliged;
    if (priv_str == "priviliged") priviliged = true;
    else if (priv_str == "unpriviliged") priviliged = false;
    else throw std::invalid_argument("Invalid privilige_str");
    MSInterface::SharedMemoryBoardReader reader(shm_id, priviliged);
    std::string stream;
    while ((std::cin >> stream)) {
        if (stream=="end") break;
        MSInterface::BoardData data = reader.get_full_state();
        std::cout << (int)data.rows << " " << (int)data.columns << '\n';
        if (priviliged) {
            for (int i=0;i<data.rows;i++) {
                for (int j=0;j<data.rows;j++) {
                    std::cout <<( data.states[i*data.columns + data.rows].has_mine ? 'M' : ' ');
                }
                std::cout << '\n';
            }
            for (int i=0;i<data.rows;i++) {
                for (int j=0;j<data.rows;j++) {
                    std::cout <<( data.states[i*data.columns + data.rows].mine_count ? 'M' : ' ');
                }
                std::cout << '\n';
            }
            for (int i=0;i<data.rows;i++) {
                for (int j=0;j<data.rows;j++) {
                    std::cout <<( data.states[i*data.columns + data.rows].is_revealed ? 'M' : ' ');
                }
                std::cout << '\n';
            }
        }
        else {
            for (int i=0;i<data.rows;i++) {
                for (int j=0;j<data.rows;j++) {
                    MinesweeperLogic::cell state = data.states[i*data.columns + data.rows];
                    if (!state.is_revealed) std::cout << "#";
                    else                   std::cout << (int)state.mine_count;
                }
                std::cout << '\n';
            }
        }
        std::cout << std::flush;
    }
}