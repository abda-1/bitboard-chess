#include <SDL2/SDL.h>
#include <iostream>
#include <UI.hpp>
#include <Board.hpp>

int main(int argc, char *argv[]){

    // initialise SDL

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        std::cerr << "SDL could not initalise! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // create window
    int squareSize = 75;

    SDL_Window* window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, squareSize*8, squareSize*8, SDL_WINDOW_ALLOW_HIGHDPI);

    // check that window has been properly instantiated
    if(window == NULL){
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // create board and ui instances
    Board chessBoard;
    UI ui(renderer, &chessBoard, squareSize);
    ui.loadImages();

    // initialise main game loop
    SDL_Event windowEvent;
    while(true){
        
        // event handling
        if(SDL_PollEvent(&windowEvent)){
            
            // quit condition
            if(SDL_QUIT == windowEvent.type) break;

        }

        // render the board
        SDL_RenderClear(renderer);
        ui.drawChessboard();
        ui.drawPieces();
        SDL_RenderPresent(renderer);
        
    }

    // cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;

}