
#include "Board.hpp"
#include <cassert>
#include <iostream>
#include <AudioManager.hpp>
#include <SDL.h>
#include <SDL_mixer.h>

void print(U64 board){
    for(int rank = 8; rank >= 1; rank--) {
        for(int file = 1; file <= 8; file++) {
            int square = (rank - 1) * 8 + (file - 1);
            std::cout << ((board >> square) & 1) << " ";
        }
        std::cout << std::endl;
    }
}



int main(int argc, char* argv[]) {
    AudioManager audio;
    return 0;
}
