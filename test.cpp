
#include "Board.hpp"
#include "MoveGenerator.hpp"
#include <cassert>
#include <iostream>

void print(U64 board){
    for(int rank = 8; rank >= 1; rank--) {
        for(int file = 1; file <= 8; file++) {
            int square = (rank - 1) * 8 + (file - 1);
            std::cout << ((board >> square) & 1) << " ";
        }
        std::cout << std::endl;
    }
}

void testMoveGeneration() {

    Board board;
    board.clearBoard();

    MoveGenerator mg(board);
    
    U64 validMoves = 0;

}

int main(){

    testMoveGeneration();

    return 0;

}
