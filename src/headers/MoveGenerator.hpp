#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include <unordered_map>
#include <PieceType.hpp>
#include <Board.hpp>
#include <BitOperations.hpp>
#include <utility>
#include <vector>

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
        
        // // Use a cache to prevent multiple recalculations of the same attacks upon each move
        // U64 moveCache;
        // U64 attackCache;
        // bool cacheUpToDate = false;

        // Move Generation
        // Returned BitBoard will include all possible locations that piece can be moved to.
        U64 generatePawnMoves(int position, bool isWhite, bool includeBlocker);
        U64 generateKnightMoves(int position, bool isWhite, bool includeBlocker);
        U64 generateRookMoves(int position, bool isWhite, bool includeBlocker);
        U64 generateBishopMoves(int position, bool isWhite, bool includeBlocker);
        U64 generateQueenMoves(int position, bool isWhite, bool includeBlocker);
        U64 generateKingMoves(int position, bool isWhite, bool includeBlocker);

        U64 generatePieceMovesOrAttacks (PieceType pieceType, int position, bool attack);
        U64 generateAllMovesOrAttacks (bool isWhite, bool attack, Board& board);

        // includeblocker  -> return squares the piece can attack
        // !includeblocker -> return only the valid move squares

        // Valid Move Detection Helpers
        std::vector<pair<PieceType, int>> findCheckingPieces(bool isWhite, Board& board);
        U64 generateLineOfAttack(int checkingPiecePosition, int kingPosition);
        void updatePieces(Board& board);
        bool isKingInCheck (bool isWhite, Board& currentBoard);

    public:
        MoveGenerator(Board& board);

        void updatePieces();
        bool isKingInCheck (bool isWhite);
        U64 generatePieceValidMoves (PieceType pieceType, int position);
        
};

U64 MoveGenerator::generatePieceValidMoves(PieceType pieceType, int position) {

    updatePieces();

    // Determine is the king is currently in check
    U64 validMoves = 0;
    U64 isWhite = pieceTypeToString(pieceType)[0] == 'W';
    U64 opponentPieces = isWhite ? blackPieces : whitePieces;
    bool kingInCheck = isKingInCheck(isWhite);

    // Generate all the currently possible moves
    U64 possibleMoves = generatePieceMovesOrAttacks(pieceType, position, false);
    U64 possibleMovesAndAttacks = (generatePieceMovesOrAttacks(pieceType, position, true) & opponentPieces) | possibleMoves;

    // King Specific Logic
    if (pieceType == PieceType::WK || pieceType == PieceType::BK) {
        if(kingInCheck) {
            U64 opponentAttacks = generateAllMovesOrAttacks(!isWhite, true, this->chessBoard);
            return possibleMovesAndAttacks & ~opponentAttacks;
        }
    }

    // Iterate through all possible moves, and check whether the move leaves the king in check. 
    while (possibleMovesAndAttacks) {

        // Find LSB and clear
        int toPos = findLSBIndex(possibleMovesAndAttacks);
        possibleMovesAndAttacks &= ~(1ULL << toPos);

        U64 oldWhitePieces = whitePieces;
        U64 oldBlackPieces = blackPieces;

        // Simulate the move being executed
        Board tempBoard = chessBoard;
        tempBoard.executeMove(pieceType, position, toPos);  // Fix sound issue here ? 
        updatePieces(tempBoard);
        
        if (!isKingInCheck(isWhite, tempBoard)) validMoves |= (1ULL << toPos);
        
        // Reset move execution
        whitePieces = oldWhitePieces;
        blackPieces = oldBlackPieces;

    }

    return validMoves;

}

U64 MoveGenerator::generateLineOfAttack(int checkingPiecePosition, int kingPosition) {

    U64 lineOfAttack = 0;

    // Helpers to determine direction of line of attack
    int rankDiff = kingPosition / 8 - checkingPiecePosition / 8;
    int fileDiff = kingPosition % 8 - checkingPiecePosition % 8;

    // Determine the 9 direction that could be possible from king position -> checkingPiece

    // North
    if (fileDiff == 0 && rankDiff < 0) {
        for (int pos = kingPosition + 8; pos <= checkingPiecePosition; pos += 8) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // North East
    else if (fileDiff < 0 && rankDiff < 0) {
        for (int pos = kingPosition + 9; pos <= checkingPiecePosition; pos += 9) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // North West
    else if (fileDiff > 0 && rankDiff < 0) {
        for (int pos = kingPosition + 7; pos <= checkingPiecePosition; pos += 7) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // South
    else if (fileDiff == 0 && rankDiff > 0) {
        for (int pos = kingPosition - 8; pos >= checkingPiecePosition; pos -= 8) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // South East
    else if (fileDiff < 0 && rankDiff > 0) {
        for (int pos = kingPosition - 7; pos >= checkingPiecePosition; pos -= 7) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // South West
    else if (fileDiff > 0 && rankDiff > 0) {
        for (int pos = kingPosition - 9; pos >= checkingPiecePosition; pos -= 9) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // West
    else if (fileDiff > 0 && rankDiff == 0) {
        for (int pos = kingPosition - 1; pos >= checkingPiecePosition; pos--) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    // East
    else if (fileDiff < 0 && rankDiff == 0) {
        for (int pos = kingPosition + 1; pos >= checkingPiecePosition; pos++) {
            lineOfAttack |= (1ULL << pos);
        }
    }

    return lineOfAttack;
    
}

// Return the piece type and position of the piece that was causing the check
std::vector<pair<PieceType, int>> MoveGenerator::findCheckingPieces(bool isWhite, Board& board) {

    std::vector<pair<PieceType, int>> checkingPieces;
    U64 kingBoard = isWhite ? board.getCurrentBoard()[PieceType::WK] : board.getCurrentBoard()[PieceType::BK];

    char sign = isWhite ? 'W' : 'B';

    for (const auto& [type, bitboard] : board.getCurrentBoard()) {
        if (pieceTypeToString(type)[0] == sign) continue;

        U64 allPieces = bitboard;

        while (allPieces) {
            int index = findLSBIndex(allPieces);
            if (index == -1) break;
            allPieces &= ~(1ULL << index);
            
            if (generatePieceMovesOrAttacks(type, index, true) & kingBoard) {
                checkingPieces.emplace_back(make_pair(type, index));
                cout << pieceTypeToString(type) << " is causing a check." << endl;  /////////// COMMENT 
            }
        }
    }
    
    cout << "there are " << checkingPieces.size() << " pieces causing check" << endl; /////////// COMMENT 
    return checkingPieces;

}

U64 MoveGenerator::generateAllMovesOrAttacks (bool isWhite, bool attack, Board& board) {

    U64 moves = 0;
    char sign = isWhite ? 'W' : 'B';

    // Iterate through every board
    for (const auto& [type, bitboard] : board.getCurrentBoard()) {
        if (pieceTypeToString(type)[0] != sign) continue;
        
        U64 allPieces = bitboard;
        
        // Iterate over the set bits and incrementally generate all moves for each piece
        while (allPieces) {
            int index = findLSBIndex(allPieces);
            if (index == -1) break;
            allPieces &= ~(1ULL << index);
            moves |= generatePieceMovesOrAttacks(type, index, attack);
        }

    }

    // Could cache before return
    return moves;

}

U64 MoveGenerator::generatePieceMovesOrAttacks (PieceType pieceType, int position, bool attack) {

    // Generate only moves if attack is false
    // Return all valid attacked squares if attack

    switch (pieceType) {

        // Black Pieces
        case PieceType::BP:
            return generatePawnMoves(position, false, attack);
        case PieceType::BB:
            return generateBishopMoves(position, false, attack);
        case PieceType::BN:
            return generateKnightMoves(position, false, attack);
        case PieceType::BR:
            return generateRookMoves(position, false, attack);
        case PieceType::BQ:
            return generateQueenMoves(position, false, attack);
        case PieceType::BK:
            return generateKingMoves(position, false, attack);

        // White Pieces
        case PieceType::WP:
            return generatePawnMoves(position, true, attack);
        case PieceType::WB:
            return generateBishopMoves(position, true, attack);
        case PieceType::WN:
            return generateKnightMoves(position, true, attack);
        case PieceType::WR:
            return generateRookMoves(position, true, attack);
        case PieceType::WQ:
            return generateQueenMoves(position, true, attack);
        case PieceType::WK:
            return generateKingMoves(position, true, attack);

        default:
            return 0;
    }


}

// Check if king of given colour is in check (for temp board)
bool MoveGenerator::isKingInCheck (bool isWhite, Board& currentBoard){

    // Obtain all the possible attacks from the opposition
    U64 opponentAttacks = generateAllMovesOrAttacks(!isWhite, true, currentBoard);
    U64 kingBoard = isWhite ? currentBoard.getCurrentBoard()[PieceType::WK] : currentBoard.getCurrentBoard()[PieceType::BK];

    return opponentAttacks & kingBoard;
}

// Default implementation of above function (for current board)
bool MoveGenerator::isKingInCheck (bool isWhite) {
    return isKingInCheck (isWhite, this->chessBoard);
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

void MoveGenerator::updatePieces(Board& board) {

    // Intialise both boards to be null
    whitePieces = 0; blackPieces = 0;

    // Iterate through every board and append to whitePieces or blackPieces
    for(const auto& [type, bitboard] : board.getCurrentBoard()) {
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

U64 MoveGenerator::generatePawnMoves (int position, bool isWhite, bool includeBlocker) {

    U64 currentPawn = 1ULL << position;
    U64 moves = 0;

    if (isWhite) {
        // Single pawn push
        U64 singlePush = (currentPawn << 8) & ~whitePieces & ~blackPieces;

        // Double pawn push for white pawn only possible if on 2nd rank and there is no piece in front (i.e. has the option to make a single push)
        
        U64 doublePush = singlePush ? ((currentPawn & RANK_2) << (8 * 2)) & ~whitePieces & ~blackPieces : 0ULL;

        // Capture left, upwards (only possible if not on the first file)
        U64 captureLeft = ((currentPawn & ~FILE_A) << 7) & ~whitePieces & blackPieces;

        // Capture right, upwards (only possible if not on the eigth file)
        U64 captureRight = ((currentPawn & ~FILE_H) << 9) & ~whitePieces & blackPieces;

        if (includeBlocker) moves = captureLeft | captureRight;
        else moves = singlePush | doublePush;
    }

    else {
        U64 singlePush = (currentPawn >> 8) & ~blackPieces & ~whitePieces;

        // Double pawn push, for black pawn only possible if on 7th rank
        U64 doublePush = singlePush ? ((currentPawn & RANK_7) >> (8 * 2)) & ~blackPieces & ~whitePieces : 0ULL;

        // Capture left, downwards - (only possible if not on the first file)
        U64 captureLeft = ((currentPawn & ~FILE_A) >> 7) & ~blackPieces & whitePieces;

        // Capture right, downwards (only possible if not on the eigth file)
        U64 captureRight = ((currentPawn & ~FILE_H) >> 9) & ~blackPieces & whitePieces;

        if (includeBlocker) moves = captureLeft | captureRight;
        else moves = singlePush | doublePush;
    }

    return moves;

}

U64 MoveGenerator::generateKnightMoves (int position, bool isWhite, bool includeBlocker) {

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
        if (includeBlocker) validMoves |= move;
        else validMoves |= (move & ~ownPieces);
    }

    return validMoves;    

}

U64 MoveGenerator::generateRookMoves (int position, bool isWhite, bool includeBlocker) {

    // There are more efficent methods for calculating the moves of sliding pieces
    // I.e., Magic bitboards... could be implemented after project is fully functional

    U64 moves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // Move in all directions
    // Thought that array is better than vector here due to (slight) performance overhead, size is known already
    int directions[] = {8, -8, 1, -1};

    for (int direction : directions) {
        for (int i = position + direction; i >= 0 && i <= 63; i += direction) {

            // Prevent wrapping around for left and right
            if (direction == 1 && i % 8 == 0) break;
            if (direction == -1 && i % 8 == 7) break;

            U64 move = 1ULL << i;
            if (move & (blackPieces | whitePieces)) {
                if (includeBlocker) moves |= move;
                break;
            }
            moves |= move;
        }
    }

    if (includeBlocker) return moves;
    return moves & ~ownPieces;

}

U64 MoveGenerator::generateBishopMoves (int position, bool isWhite, bool includeBlocker) {

    U64 moves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // Diagonal right up
    for (int i = position + 9; i <= 63 && i % 8 != 0; i += 9) {
        U64 move = 1ULL << i;
        if (move & (blackPieces | whitePieces)) {
            if (includeBlocker) moves |= move;
            break;
        }
        moves |= move;
    }

    // Diagonal left up
    for (int i = position + 7; i <= 63 && i % 8 != 7; i += 7) {
        U64 move = 1ULL << i;
        if (move & (blackPieces | whitePieces)) {
            if (includeBlocker) moves |= move;
            break;
        }
        moves |= move;
    }

    // Diagonal left down
    for (int i = position - 9; i >= 0 && (i % 8 != 7); i -= 9) {
        U64 move = 1ULL << i;
        if (move & (blackPieces | whitePieces)) {
            if (includeBlocker) moves |= move;
            break;
        }
        moves |= move;
    }

    // Diagonal right down
    for (int i = position - 7; i >= 0 && (i % 8 != 0); i -= 7) {
        U64 move = 1ULL << i;
        if (move & (blackPieces | whitePieces)) {
            if (includeBlocker) moves |= move;
            break;
        }
        moves |= move;
    }

    if (includeBlocker) return moves;
    return moves & ~ownPieces;

}

U64 MoveGenerator::generateQueenMoves (int position, bool isWhite, bool includeBlocker) {
    return generateRookMoves(position, isWhite, includeBlocker) | generateBishopMoves(position, isWhite, includeBlocker);
}

U64 MoveGenerator::generateKingMoves (int position, bool isWhite, bool includeBlocker) {

    U64 currentKing = 1ULL << position;
    U64 moves = 0;
    U64 ownPieces = isWhite ? whitePieces : blackPieces;

    // King will move in all directions 1 square away
    moves |= ((currentKing & ~RANK_8) << 8);               // Up
    moves |= ((currentKing & ~RANK_1) >> 8);               // Down
    
    moves |= ((currentKing & ~FILE_A) << 1);               // Left
    moves |= ((currentKing & ~FILE_H) >> 1);               // Right
    
    moves |= ((currentKing & ~(FILE_A | RANK_8)) << 9);   // Up Left
    moves |= ((currentKing & ~(FILE_H | RANK_8)) << 7);   // Up Right
    
    moves |= ((currentKing & ~(FILE_A | RANK_1)) >> 9);   // Down Left
    moves |= ((currentKing & ~(FILE_H | RANK_1)) >> 7);   // Down Right

    if (includeBlocker) return moves;
    return (moves & ~ownPieces);
    
}

#endif // MOVEGENERATOR_H