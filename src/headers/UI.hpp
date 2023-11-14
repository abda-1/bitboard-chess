#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

/*
    UI Class Responsibilites (class that works with SDL2)
        - generate game state (chessboard)
        - capture user input
        - display game status (i.e. turn / checkmate / evaluation)
            
*/

class UI{

    private:
        const int SQUARE_SIZE = 100;
        const int BOARD_SIZE = SQUARE_SIZE * 8;

    public:
        void drawChessboard(SDL_Renderer* r);
        int getBoardSize();

};

int UI::getBoardSize(){
    return BOARD_SIZE;
}

void UI::drawChessboard(SDL_Renderer* r){

    SDL_Rect square;

    square.w = SQUARE_SIZE;
    square.h = SQUARE_SIZE;

    // set up board to be alternating colours
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){

            if ((i+j) % 2 == 0) SDL_SetRenderDrawColor(r,233,237,204,255);  // colour 1
            else                SDL_SetRenderDrawColor(r,119,153,84,255);   // colour 2

            square.x = j * SQUARE_SIZE;                     // x-coordinate of square
            square.y = i * SQUARE_SIZE;                     // y-coordinate of square

            SDL_RenderFillRect(r, &square);
        }
    }

}









#endif  // UI_H