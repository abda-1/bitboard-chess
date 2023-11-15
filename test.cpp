
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

    MoveGenerator mg(board.getCurrentBoard());
    
    U64 validPawnMoves = mg.generateWhitePawn();

    std::cout << "White pawn valid moves: \n";
    print(validPawnMoves);

}

int main(){

    testMoveGeneration();

    return 0;

}
