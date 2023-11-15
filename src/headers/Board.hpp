/*

    - comprised of the 12 individual bitboards 

*/

#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <PieceType.hpp>
#include <unordered_map>
#include <iostream>

using namespace std;
typedef uint64_t U64;


class Board{

    private:
        unordered_map<PieceType, U64> currentBoard;
        void initialiseBoard();

        // helper
        void printU64(U64 board);

    public:
        Board();
        unordered_map<PieceType, U64> getCurrentBoard();
        
        



};

// helper
void Board::printU64(U64 board){

    for(int rank = 8; rank >= 1; rank--) {
        for(int file = 1; file <= 8; file++) {
            int square = (rank - 1) * 8 + (file - 1);
            std::cout << ((board >> square) & 1) << " ";
        }
        std::cout << std::endl;
    }

}

Board::Board(){
    initialiseBoard();
}

void Board::initialiseBoard(){

    // starting positions for the white pieces
    currentBoard[PieceType::WP] = 0xFF00ULL;
    currentBoard[PieceType::WR] = 0x81ULL;
    currentBoard[PieceType::WN] = 0x42ULL;
    currentBoard[PieceType::WB] = 0x24ULL;
    currentBoard[PieceType::WQ] = 0x08ULL;
    currentBoard[PieceType::WK] = 0x10ULL;

    // black pieces are shifted "upwards"
    currentBoard[PieceType::BP] = 0xFF00ULL << 8 * 5;
    currentBoard[PieceType::BR] = 0x81ULL << 8 * 7;
    currentBoard[PieceType::BN] = 0x42ULL << 8 * 7;
    currentBoard[PieceType::BB] = 0x24ULL << 8 * 7;
    currentBoard[PieceType::BQ] = 0x08ULL << 8 * 7;
    currentBoard[PieceType::BK] = 0x10ULL << 8 * 7;

    // for(auto board : currentBoard){
    //     cout << pieceTypeToString(board.first) << " :" << endl;
    //     printU64(board.second);
    //     cout << endl;
    // }


}

unordered_map<PieceType, U64> Board::getCurrentBoard(){
    return currentBoard;
}


#endif // BOARD_H