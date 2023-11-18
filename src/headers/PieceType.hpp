#ifndef PIECETYPE_H
#define PIECETYPE_H

#include <string>
#include <unordered_map>

enum class PieceType{

    BP, BR, BB, BN, BQ, BK, WP, WR, WB, WN, WQ, WK, EMPTY

};

enum class BlackPiece{

    BP, BR, BB, BN, BQ, BK

};

enum class WhitePiece{

    WP, WR, W, WN, WQ, WK

};

std::string pieceTypeToString(PieceType type) {
    static std::unordered_map<PieceType, std::string> pieceTypeNames = {
        {PieceType::BP, "Black Pawn"},
        {PieceType::BR, "Black Rook"},
        {PieceType::BB, "Black Bishop"},
        {PieceType::BN, "Black Knight"},
        {PieceType::BQ, "Black Queen"},
        {PieceType::BK, "Black King"},
        {PieceType::WP, "White Pawn"},
        {PieceType::WR, "White Rook"},
        {PieceType::WB, "White Bishop"},
        {PieceType::WN, "White Knight"},
        {PieceType::WQ, "White Queen"},
        {PieceType::WK, "White King"},
        {PieceType::EMPTY, "Empty"}
    };

    auto it = pieceTypeNames.find(type);
    if (it != pieceTypeNames.end()) {
        return it->second;
    } else {
        return "Unknown PieceType";
    }
}

#endif // PIECETYPE_H