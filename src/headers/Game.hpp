#ifndef GAME_H
#define GAME_H

/*
    Game Class Responsibilites (class that works with SDL2)
        - hold state of the game (after move has been completed)
        - interacts with all other classes to control flow of the game
            
*/

#include "Board.hpp"
#include "MoveGenerator.hpp"
class Game{

    private:
        Board chessBoard;
        MoveGenerator MoveGenerator;
        bool whiteTurn;

    public:
        Game();

        bool isWhiteTurn();
        void switchTurn();
        bool isCheck(bool whiteSide);
        



};

Game::Game() : MoveGenerator(chessBoard), whiteTurn(true) {
}

bool Game::isWhiteTurn(){
    return whiteTurn;
}

void Game::switchTurn(){
    whiteTurn = !whiteTurn;
}





#endif // GAME_H