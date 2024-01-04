#include <MoveGenerator.hpp>
#include <Board.hpp>

int main(int argc, char *argv[]){

    AudioManager a;
    Board b(a);
    MoveGenerator mg(b);

    b.clearBoard();

    int kingPos = 19;
    int other = 18;

    b.addPiece(PieceType::WK, 20);
    b.addPiece(PieceType::BQ, 41);

    b.printU64(3);

    return 0;

}