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

        U64 generateWhitePawn(int position);
        U64 generateBlackPawn(int position);

        U64 generateWhiteKnight(int position);
        U64 generateBlackKnight(int position);

        U64 generateWhiteRook(int position);
        U64 generateBlackRook(int position);

        U64 generateWhiteBishop(int position);
        U64 generateBlackBishop(int position);
        
        U64 generateWhiteQueen(int position);
        U64 generateBlackQueen(int position);
        
        U64 generateWhiteKing(int position);
        U64 generateBlackKing(int position);
        
    public:
        MoveGenerator(Board& board);
        void updatePieces();

        U64 generateMoves(PieceType pieceType, int position) {

            // Return the generated moves based on the pieceType 
            switch (pieceType) {

                // Black Pieces
                case PieceType::BP:
                    return generateBlackPawn(position);
                case PieceType::BR:
                    return generateBlackRook(position);
                case PieceType::BB:
                    return generateBlackBishop(position);
                case PieceType::BN:
                    return generateBlackKnight(position);
                case PieceType::BQ:
                    return generateBlackQueen(position);
                case PieceType::BK:
                    return generateBlackKing(position);

                // White Pieces
                case PieceType::WP:
                    return generateWhitePawn(position);
                case PieceType::WR:
                    return generateWhiteRook(position);
                case PieceType::WB:
                    return generateWhiteBishop(position);
                case PieceType::WN:
                    return generateWhiteKnight(position);
                case PieceType::WQ:
                    return generateWhiteQueen(position);
                case PieceType::WK:
                    return generateWhiteKing(position);

                default:
                    return 0;
            
            }

        }

};


void MoveGenerator::updatePieces() {

    // Intialise both boards to be null
    whitePieces = 0; blackPieces = 0;

    // Iterate through every board and append to whitePieces or blackPieces
    for(const auto& [type, bitboard] : chessBoard.getCurrentBoard()) {
        char colour = pieceTypeToString(type)[0];

        if (colour == 'W') {
            whitePieces |= bitboard;
        }

        else {
            blackPieces |= bitboard;
        }

    }

}

MoveGenerator::MoveGenerator(Board& board) : chessBoard(board) {

    // Initialise white pieces board and black pieces board.
    U64 white = 0, black = 0;

    for(const auto& [type, bitb] : board.getCurrentBoard()){

        char colour = pieceTypeToString(type)[0];

        if(colour == 'W'){
            white |= bitb;
        }
        
        else {
            black |= bitb;
        }

    }

    whitePieces = white;
    blackPieces = black;

}

U64 MoveGenerator::generateWhitePawn (int position) {

    updatePieces();

    U64 currentPawn = 1ULL << position;
    U64 validMoves;

    // single pawn push
    U64 singlePush = (currentPawn << 8) & ~whitePieces & ~blackPieces;

    // double pawn push for white pawn only possible if on 2nd rank
    U64 doublePush = ((currentPawn & RANK_2) << 8 * 2) & ~whitePieces & ~blackPieces;

    // capture left (only possible if not on the first file)
    U64 captureLeft = ((currentPawn & ~FILE_A) << 7) & ~whitePieces & blackPieces;

    // capture right (only possible if not on the eigth file)
    U64 captureRight = ((currentPawn & ~FILE_H) << 9) & ~whitePieces & blackPieces;

    validMoves = singlePush | doublePush | captureLeft | captureRight;
    return validMoves;

}

U64 MoveGenerator::generateBlackPawn (int position) {

    updatePieces();

    U64 currentPawn = 1ULL << position;
    U64 validMoves;

    // single pawn push
    U64 singlePush = (currentPawn >> 8) & ~blackPieces & ~whitePieces;

    // double pawn push, for black pawn only possible if on 7th rank
    U64 doublePush = ((currentPawn & RANK_7) >> 8 * 2) & ~blackPieces & ~whitePieces;

    // capture left, downwards - (only possible if not on the first file)
    U64 captureLeft = ((currentPawn & ~FILE_A) >> 7) & ~blackPieces & whitePieces;

    // capture right, downwards (only possible if not on the eigth file)
    U64 captureRight = ((currentPawn & ~FILE_H) >> 9) & ~blackPieces & whitePieces;

    validMoves = singlePush | doublePush | captureLeft | captureRight;
    return validMoves;

}

U64 MoveGenerator::generateWhiteKnight (int position) {

    updatePieces();

    U64 currentKnight = 1ULL << position;
    U64 validMoves = 0;

    // eight moves in total, two in each quadrant
    U64 moves[8];

    // quadrant 1
    moves[0] = (currentKnight & ~FILE_H) << 17;                 // 2 up 1 right
    moves[1] = (currentKnight & ~(FILE_G | FILE_H)) << 10;      // 1 up 2 right

    // // quadrant 2
    moves[2] = (currentKnight & ~(FILE_G | FILE_H)) >> 6;       // 1 down 2 right
    moves[3] = (currentKnight & ~FILE_H) >> 15;                 // 2 down 1 right

    // // quadrant 3
    moves[4] = (currentKnight & ~FILE_A) >> 17;                 // 2 down 1 left
    moves[5] = (currentKnight & ~(FILE_A | FILE_B)) >> 10;      // 1 down 2 left

    // // quadrant 4
    moves[6] = (currentKnight & ~(FILE_A | FILE_B)) << 6;       // 1 up 2 left
    moves[7] = (currentKnight & ~FILE_A) << 15;                 // 2 up 1 left

    // exlude moves that are landing on white pieces
    for (auto move : moves){
        validMoves |= (move & ~whitePieces);
    }

    return validMoves;

}

U64 MoveGenerator::generateBlackKnight (int position) {

    updatePieces();

    U64 currentKnight = 1ULL << position;
    U64 validMoves = 0;

    // eight moves in total, two in each quadrant
    U64 moves[8];

    // quadrant 1
    moves[0] = (currentKnight & ~FILE_H) << 17;                 // 2 up 1 right
    moves[1] = (currentKnight & ~(FILE_G | FILE_H)) << 10;      // 1 up 2 right

    // // quadrant 2
    moves[2] = (currentKnight & ~(FILE_G | FILE_H)) >> 6;       // 1 down 2 right
    moves[3] = (currentKnight & ~FILE_H) >> 15;                 // 2 down 1 right

    // // quadrant 3
    moves[4] = (currentKnight & ~FILE_A) >> 17;                 // 2 down 1 left
    moves[5] = (currentKnight & ~(FILE_A | FILE_B)) >> 10;      // 1 down 2 left

    // // quadrant 4
    moves[6] = (currentKnight & ~(FILE_A | FILE_B)) << 6;       // 1 up 2 left
    moves[7] = (currentKnight & ~FILE_A) << 15;                 // 2 up 1 left

    // exlude moves that are landing on black pieces
    for (auto move : moves){
        validMoves |= (move & ~blackPieces);
    }

    return validMoves;

}

U64 MoveGenerator::generateWhiteRook (int position) {

    // there are more efficent methods for calculating the moves of sliding pieces -> magic bitboards (could be implemented after project is fully functional)

    updatePieces();

    U64 currentRook = 1ULL << position;
    U64 validMoves = 0;

    // need to handle collision of pieces

    // move up
    for (int i = position + 8; i <= 63; i += 8) {
        U64 moveUp = 1ULL << i;
        validMoves |= moveUp;
        
        // break when collision occurs
        if (moveUp & (whitePieces | blackPieces)) break;  
    }

    // move down
    for (int i = position - 8; i >= 0; i -= 8) {
        U64 moveDown = 1ULL << i;
        validMoves |= moveDown;
        
        if (moveDown & (whitePieces | blackPieces)) break;  
    }

    // move right
    for (int i = position + 1; i % 8 != 0; i++) {
        U64 moveRight = 1ULL << i;
        validMoves |= moveRight;
        
        if (moveRight & (whitePieces | blackPieces)) break;  
    }

    // move left
    for (int i = position - 1; i % 8 != 7 && i >= 0; i--) {
        U64 moveLeft = 1ULL << i;
        validMoves |= moveLeft;
        
        if (moveLeft & (whitePieces | blackPieces)) break;  
    }

    return validMoves & ~whitePieces;

}

U64 MoveGenerator::generateBlackRook (int position) {

    updatePieces();

    U64 currentRook = 1ULL << position;
    U64 validMoves = 0;

    // need to handle collision of pieces

    // move up
    for (int i = position + 8; i <= 63; i += 8) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        // break when collision occurs
        if (move & (whitePieces | blackPieces)) break;  
    }

    // move down
    for (int i = position - 8; i >= 0; i -= 8) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    // move right
    for (int i = position + 1; i % 8 != 0; i++) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    // move left
    for (int i = position - 1; i % 8 != 7 && i >= 0; i--) {
        U64 move = 1ULL << i;
        validMoves |= move;
        
        if (move & (whitePieces | blackPieces)) break;  
    }

    return validMoves & ~blackPieces;

}

U64 MoveGenerator::generateWhiteBishop (int position) {

    updatePieces();

    U64 currentBishop = 1ULL << position;
    U64 validMoves = 0;

    // diagonal right up
    for (int i = position + 9; i <= 63 && i % 8 != 0; i += 9) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal left up
    for (int i = position + 7; i <= 63 && i % 8 != 7; i += 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal right down
    for (int i = position - 9; i >= 0 && i % 8 != 0; i -= 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal left down
    for (int i = position - 7; i >= 0 && i % 8 != 7; i -= 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    return validMoves & ~whitePieces;

}

U64 MoveGenerator::generateBlackBishop (int position) {

    updatePieces();

    U64 currentBishop = 1ULL << position;
    U64 validMoves = 0;

    // diagonal right up
    for (int i = position + 9; i <= 63 && i % 8 != 0; i += 9) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal left up
    for (int i = position + 7; i <= 63 && i % 8 != 7; i += 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal right down
    for (int i = position - 9; i >= 0 && i % 8 != 0; i -= 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    // diagonal left down
    for (int i = position - 7; i >= 0 && i % 8 != 7; i -= 7) {
        U64 move = 1ULL << i;
        validMoves |= move;
        if(move & (whitePieces | blackPieces)) break;
    }

    return validMoves & ~blackPieces;

}

U64 MoveGenerator::generateWhiteQueen (int position) {
    return generateWhiteRook(position) | generateWhiteBishop(position);
}

U64 MoveGenerator::generateBlackQueen (int position) {
    return generateBlackRook(position) | generateBlackBishop(position);
}

U64 MoveGenerator::generateWhiteKing (int position) {

    updatePieces();

    U64 currentKing = 1ULL << position;
    U64 validMoves = 0;

    // king will move in all directions 1 square away.
    validMoves |= ((currentKing & ~RANK_8) << 8);               // up
    validMoves |= ((currentKing & ~RANK_1) >> 8);               // down
    
    validMoves |= ((currentKing & ~FILE_A) << 1);               // left
    validMoves |= ((currentKing & ~FILE_H) >> 1);               // right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_8)) << 9);   // up left
    validMoves |= ((currentKing & ~(FILE_H | RANK_8)) << 7);   // up right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_1)) >> 9);   // down left
    validMoves |= ((currentKing & ~(FILE_H | RANK_1)) >> 7);   // down right

    return validMoves & ~whitePieces;

}

U64 MoveGenerator::generateBlackKing (int position) {

    updatePieces();

    U64 currentKing = 1ULL << position;
    U64 validMoves = 0;

    // king will move in all directions 1 square away.
    validMoves |= ((currentKing & ~RANK_8) << 8);               // up
    validMoves |= ((currentKing & ~RANK_1) >> 8);               // down
    
    validMoves |= ((currentKing & ~FILE_A) << 1);               // left
    validMoves |= ((currentKing & ~FILE_H) >> 1);               // right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_8)) << 9);   // up left
    validMoves |= ((currentKing & ~(FILE_H | RANK_8)) << 7);   // up right
    
    validMoves |= ((currentKing & ~(FILE_A | RANK_1)) >> 9);   // down left
    validMoves |= ((currentKing & ~(FILE_H | RANK_1)) >> 7);   // down right

    return validMoves & ~blackPieces;

}

#endif // MOVEGENERATOR_H