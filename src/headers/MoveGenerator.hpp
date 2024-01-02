#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include <unordered_map>
#include <PieceType.hpp>
#include <Board.hpp>

// Constants to assist with preventing generating moves that 'wrap' around the board
const U64 RANK_8 = 0xFF00000000000000;
const U64 RANK_7 = 0x00FF000000000000;
const U64 RANK_2 = 0x000000000000FF00;
const U64 RANK_1 = 0x00000000000000FF;

const U64 FILE_A = 0x0101010101010101;
const U64 FILE_B = 0x0202020202020200;
const U64 FILE_G = 0x4040404040404040;
const U64 FILE_H = 0x8080808080808080;


class MoveGenerator{

    private:
        Board& chessBoard;
        U64 whitePieces, blackPieces;

        U64 generatePawnMoves(int position, bool isWhite);
        U64 generateKnightMoves(int position, bool isWhite);
        U64 generateRookMoves(int position, bool isWhite);
        U64 generateBishopMoves(int position, bool isWhite);
        U64 generateQueenMoves(int position, bool isWhite);
        U64 generateKingMoves(int position, bool isWhite);

        
    public:
        MoveGenerator(Board& board);
        void updatePieces();
        U64 generateMoves (PieceType pieceType, int position);
        

};

// Generate all possible moves
U64 MoveGenerator::generateMoves(PieceType pieceType, int position) {

    // Return the generated moves based on the pieceType 
    switch (pieceType) {

        // Black Pieces
        case PieceType::BP:
            return generatePawnMoves(position, false);
        case PieceType::BR:
            return generateRookMoves(position, false);
        case PieceType::BB:
            return generateBishopMoves(position, false);
        case PieceType::BN:
            return generateKnightMoves(position, false);
        case PieceType::BQ:
            return generateQueenMoves(position, false);
        case PieceType::BK:
            return generateKingMoves(position, false);

        // White Pieces
        case PieceType::WP:
            return generatePawnMoves(position, true);
        case PieceType::WR:
            return generateRookMoves(position, true);
        case PieceType::WB:
            return generateBishopMoves(position, true);
        case PieceType::WN:
            return generateKnightMoves(position, true);
        case PieceType::WQ:
            return generateQueenMoves(position, true);
        case PieceType::WK:
            return generateKingMoves(position,  true);

        default:
            return 0;

    }

}


// Check where this has been implemented for future.... calling each time is def not efficient.  
void MoveGenerator::updatePieces() {

    // Intialise both boards to be null
    whitePieces = 0; blackPieces = 0;

    // Iterate through every board and append to whitePieces or blackPieces
    for(const auto& [type, bitboard] : chessBoard.getCurrentBoard()) {
        char colour = pieceTypeToString(type)[0];

        if (colour == 'W') whitePieces |= bitboard;
        else blackPieces |= bitboard;
    }

}

MoveGenerator::MoveGenerator(Board& board) : chessBoard(board) {

    // Initialise white pieces board and black pieces board.
    U64 white = 0, black = 0;

    for(const auto& [type, bitb] : board.getCurrentBoard()){

        char colour = pieceTypeToString(type)[0];

        if(colour == 'W') white |= bitb;
        else black |= bitb;

    }

    whitePieces = white;
    blackPieces = black;

}

U64 MoveGenerator::generatePawnMoves (int position, bool isWhite) {

    U64 currentPawn = 1ULL << position;
    U64 validMoves = 0;

    if (isWhite) {
        // Single pawn push
        U64 singlePush = (currentPawn << 8) & ~whitePieces & ~blackPieces;

        // Double pawn push for white pawn only possible if on 2nd rank
        U64 doublePush = ((currentPawn & RANK_2) << (8 * 2)) & ~whitePieces & ~blackPieces;

        // Capture left, upwards (only possible if not on the first file)
        U64 captureLeft = ((currentPawn & ~FILE_A) << 7) & ~whitePieces & blackPieces;

        // Capture right, upwards (only possible if not on the eigth file)
        U64 captureRight = ((currentPawn & ~FILE_H) << 9) & ~whitePieces & blackPieces;

        validMoves = singlePush | doublePush | captureLeft | captureRight;
    }

    else {
        U64 singlePush = (currentPawn >> 8) & ~blackPieces & ~whitePieces;

        // Double pawn push, for black pawn only possible if on 7th rank
        U64 doublePush = ((currentPawn & RANK_7) >> (8 * 2)) & ~blackPieces & ~whitePieces;

        // Capture left, downwards - (only possible if not on the first file)
        U64 captureLeft = ((currentPawn & ~FILE_A) >> 7) & ~blackPieces & whitePieces;

        // Capture right, downwards (only possible if not on the eigth file)
        U64 captureRight = ((currentPawn & ~FILE_H) >> 9) & ~blackPieces & whitePieces;

        validMoves = singlePush | doublePush | captureLeft | captureRight;
    }

    return validMoves;

}

U64 MoveGenerator::generateKnightMoves (int position, bool isWhite) {

    U64 currentKnight = 1ULL << position;
    U64 validMoves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // Eight moves in total, two in each quadrant
    U64 moves[8];

    // Qadrant 1
    moves[0] = (currentKnight & ~FILE_H) << 17;                 // 2 up 1 right
    moves[1] = (currentKnight & ~(FILE_G | FILE_H)) << 10;      // 1 up 2 right

    // Quadrant 2
    moves[2] = (currentKnight & ~(FILE_G | FILE_H)) >> 6;       // 1 down 2 right
    moves[3] = (currentKnight & ~FILE_H) >> 15;                 // 2 down 1 right

    // Quadrant 3
    moves[4] = (currentKnight & ~FILE_A) >> 17;                 // 2 down 1 left
    moves[5] = (currentKnight & ~(FILE_A | FILE_B)) >> 10;      // 1 down 2 left

    // Quadrant 4
    moves[6] = (currentKnight & ~(FILE_A | FILE_B)) << 6;       // 1 up 2 left
    moves[7] = (currentKnight & ~FILE_A) << 15;                 // 2 up 1 left

    // Exclude moves that are landing on own pieces
    for (auto move : moves){
        validMoves |= (move & ~ownPieces);
    }

    return validMoves;    

}

U64 MoveGenerator::generateRookMoves (int position, bool isWhite) {

    // There are more efficent methods for calculating the moves of sliding pieces
    // I.e., Magic bitboards... could be implemented after project is fully functional

    U64 validMoves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // Move up
    for (int i = position + 8; i <= 63; i += 8) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        // Break when collision occurs
        if (move & (whitePieces | blackPieces)) break;  
    }

    // Move down
    for (int i = position - 8; i >= 0; i -= 8) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    // Move right
    for (int i = position + 1; i % 8 != 0; i++) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    // Move left
    for (int i = position - 1; i % 8 != 7 && i >= 0; i--) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    return validMoves & ~ownPieces;

}

U64 MoveGenerator::generateBishopMoves (int position, bool isWhite) {

    U64 validMoves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // Diagonal right up
    for (int i = position + 9; i <= 63 && i % 8 != 0; i += 9) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // Diagonal left up
    for (int i = position + 7; i <= 63 && i % 8 != 7; i += 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // Diagonal left down
    for (int i = position - 9; i >= 0 && (i % 8 != 7); i -= 9) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // Diagonal right down
    for (int i = position - 7; i >= 0 && (i % 8 != 0); i -= 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    return validMoves & ~ownPieces;

}

U64 MoveGenerator::generateQueenMoves (int position, bool isWhite) {
    return generateRookMoves(position, isWhite) | generateBishopMoves(position, isWhite);
}

U64 MoveGenerator::generateKingMoves (int position, bool isWhite) {

    U64 currentKing = 1ULL << position;
    U64 validMoves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // King will move in all directions 1 square away
    validMoves |= ((currentKing & ~RANK_8) << 8);               // Up
    validMoves |= ((currentKing & ~RANK_1) >> 8);               // Down
    
    validMoves |= ((currentKing & ~FILE_A) << 1);               // Left
    validMoves |= ((currentKing & ~FILE_H) >> 1);               // Right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_8)) << 9);   // Up Left
    validMoves |= ((currentKing & ~(FILE_H | RANK_8)) << 7);   // Up Right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_1)) >> 9);   // Down Left
    validMoves |= ((currentKing & ~(FILE_H | RANK_1)) >> 7);   // Down Right

    return validMoves & ~ownPieces;
    
}

#endif // MOVEGENERATOR_H