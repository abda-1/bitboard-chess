#include <SDL2/SDL.h>
#include <iostream>
#include <UI.hpp>

const int WIDTH = 800, HEIGHT = 600;

int main(int argc, char *argv[]){

    UI UI;

    // initialise
    SDL_Init(SDL_INIT_EVERYTHING);

    // error when sdl cannot initialise for some reason
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL could not initalise! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, UI.getBoardSize(), UI.getBoardSize(), SDL_WINDOW_ALLOW_HIGHDPI);

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

    // initialise main game loop
    SDL_Event windowEvent;

    while(true){
        
        // quit condition
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type) break;
        }

        SDL_RenderClear(renderer);
        UI.drawChessboard(renderer);
        SDL_RenderPresent(renderer);
        
    }

    // cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;

}