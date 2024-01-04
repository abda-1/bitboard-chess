#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <PieceType.hpp>
#include <unordered_map>
#include <iostream>
#include <AudioManager.hpp>

using namespace std;
typedef uint64_t U64;

// Default starting setup

class Board{

    private:
        unordered_map<PieceType, U64> currentBoard;
        void initialiseBoard();
        AudioManager& audioManager;

    public:
        Board(AudioManager& audioManager);
        Board(AudioManager& a, unordered_map<PieceType,U64> customBoard);

        // Board move helpers
        void movePiece(PieceType type, int fromPos, int toPos);
        void capturePiece(PieceType type, int position);
        void executeMove(PieceType selectedPiece, int fromPos, int toPos);
        bool isOpponentPiece(PieceType pieceOne, PieceType pieceTwo);

        // Utility 
        PieceType getPieceAtPosition (int position);
        void clearBoard();
        void printU64(U64 board);
        unordered_map<PieceType, U64> getCurrentBoard();

        // Custom board creation
        // Testing functions
        void addPiece(PieceType type, int position);
        void removePiece (PieceType type, int position);
        
};

void Board::addPiece (PieceType type, int position) {
    currentBoard[type] |= (1ULL << position);
}

void Board::removePiece (PieceType type, int position) {
    currentBoard[type] && ~(1ULL << position);
}

Board::Board(AudioManager& a) : audioManager(a) {
    initialiseBoard();
    a.playSound(AudioType::START);
}

Board::Board(AudioManager& a, unordered_map<PieceType,U64> customBoard) : audioManager(a) {
    currentBoard = customBoard;
    a.playSound(AudioType::START);
}

void Board::executeMove (PieceType selectedPiece, int fromPos, int toPos) {

    // Check if move is a capture move
    PieceType capturedPiece = getPieceAtPosition(toPos);
    if (capturedPiece != PieceType::EMPTY && isOpponentPiece(selectedPiece, capturedPiece)) {
        capturePiece(capturedPiece, toPos);
        movePiece(selectedPiece, fromPos, toPos);
        audioManager.playSound(AudioType::CAPTURE);
    } else {
        movePiece(selectedPiece, fromPos, toPos);
        audioManager.playSound(AudioType::MOVE);
    }

}

PieceType Board::getPieceAtPosition (int position) {

    U64 location = 1ULL << position;

    // Iterate through each board type
    for (const auto& [type, bitboard] : currentBoard) {
        if (bitboard & location) {
            return type;
        }
    }

    return PieceType::EMPTY;
}

void Board::clearBoard(){

    for(auto& [type, bitboard] : currentBoard){
        bitboard = 0;
    }

}

bool Board::isOpponentPiece(PieceType pieceOne, PieceType pieceTwo){

    // Obtain types of pieces through helper function
    char colourOne = pieceTypeToString(pieceOne)[0];
    char colourTwo = pieceTypeToString(pieceTwo)[0];

    return colourOne != colourTwo;
    
}


void Board::capturePiece(PieceType type, int position){

    if(type != PieceType::EMPTY){
        U64& bitboard = currentBoard[type];
        U64 mask = 1ULL << position;
        
        // Clear the bit at the position
        bitboard &= ~mask;
    }
}

void Board::movePiece(PieceType type, int fromPos, int toPos){

    U64& bitboard = currentBoard[type];
    U64 fromMask = 1ULL << fromPos;
    U64 toMask = 1ULL << toPos;
    
    // Clear the bit at the original location
    bitboard &= ~fromMask;

    // Set the bit at the new location
    bitboard |= toMask;
}

// Print the board (debugging purposes)
void Board::printU64(U64 board){

    for(int rank = 8; rank >= 1; rank--) {
        for(int file = 1; file <= 8; file++) {
            int square = (rank - 1) * 8 + (file - 1);
            std::cout << ((board >> square) & 1) << " ";
        }
        std::cout << std::endl;
    }

}

void Board::initialiseBoard(){

    unordered_map<PieceType, U64> defaultBoard;

    // Starting positions for the white pieces
    defaultBoard[PieceType::WP] = 0xFF00ULL;
    defaultBoard[PieceType::WR] = 0x81ULL;
    defaultBoard[PieceType::WN] = 0x42ULL;
    defaultBoard[PieceType::WB] = 0x24ULL;
    defaultBoard[PieceType::WQ] = 0x08ULL;
    defaultBoard[PieceType::WK] = 0x10ULL;

    // Black pieces are shifted "upwards"
    defaultBoard[PieceType::BP] = 0xFF00ULL << 8 * 5;
    defaultBoard[PieceType::BR] = 0x81ULL << 8 * 7;
    defaultBoard[PieceType::BN] = 0x42ULL << 8 * 7;
    defaultBoard[PieceType::BB] = 0x24ULL << 8 * 7;
    defaultBoard[PieceType::BQ] = 0x08ULL << 8 * 7;
    defaultBoard[PieceType::BK] = 0x10ULL << 8 * 7;

    currentBoard = defaultBoard;

}

unordered_map<PieceType, U64> Board::getCurrentBoard(){
    return currentBoard;
}


#endif // BOARD_H