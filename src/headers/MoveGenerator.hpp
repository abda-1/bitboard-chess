#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include <unordered_map>
#include <PieceType.hpp>
#include <Board.hpp>


/*

    MoveGenerator
        - will take in 12 individual bitboards representing the chessBoard.
        - will generate moves for each piece type and then return the package of boards as an unordered_map


*/

const U64 RANK_8 = 0xFF00000000000000;
const U64 RANK_7 = 0x00FF000000000000;
const U64 RANK_2 = 0x000000000000FF00;
const U64 RANK_1 = 0x00000000000000FF;

const U64 FILE_1 = 0x0101010101010101;
const U64 FILE_8 = 0x8080808080808080;

class MoveGenerator{

    private:
        unordered_map<PieceType, U64> currentBoard;
        U64 whitePieces, blackPieces;
        
    public:
        MoveGenerator(unordered_map<PieceType, U64> board);

        U64 generateWhitePawn(int position);
        U64 generateBlackPawn();

        U64 generateWhiteKnight();
        U64 generateBlackKnight();

};

MoveGenerator::MoveGenerator(unordered_map<PieceType, U64> board){

    currentBoard = board;

    // initialise white pieces board and black pieces board.

    U64 white = 0, black = 0;

    for(const auto& [type, bitb] : currentBoard){

        char colour = pieceTypeToString(type)[0];

        if(colour == 'W'){
            white |= bitb;
        } else {
            black |= bitb;
        }

    }

    whitePieces = white;
    blackPieces = black;

}

U64 MoveGenerator::generateWhitePawn(int position){

    U64 currentPawn = 1ULL << position;
    U64 validMoves;

    // single pawn push
    U64 singlePush = (currentPawn << 8) & ~whitePieces & ~blackPieces;

    // double pawn push
    U64 doublePush = ((currentPawn & RANK_2) << 8 * 2) & ~whitePieces & ~blackPieces;

    // capture left (only possible if not on the first file)
    U64 captureLeft = ((currentPawn & ~FILE_1) << 7) & ~whitePieces & blackPieces;

    // capture right (only possible if not on the eigth file)
    U64 captureRight = ((currentPawn & ~FILE_8) << 9) & ~whitePieces & blackPieces;

    validMoves = singlePush | doublePush | captureLeft | captureRight;
    return validMoves;

}

#endif // MOVEGENERATOR_HH