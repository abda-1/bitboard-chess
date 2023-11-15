#include <SDL2/SDL.h>
#include <iostream>
#include <UI.hpp>
#include <Board.hpp>
#include <MoveGenerator.hpp>

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
    MoveGenerator moveGenerator(chessBoard.getCurrentBoard());
    UI ui(renderer, &chessBoard, squareSize);
    ui.loadImages();

    // initialise main game loop
    SDL_Event windowEvent;
    bool mouseHeld = false;
    PieceType selectedPiece;
    int selectedPieceX, selectedPieceY;

    while(true){
        
        // event handling
        if(SDL_PollEvent(&windowEvent)){
            
            // quit condition
            if(SDL_QUIT == windowEvent.type) break;

            else if (windowEvent.type == SDL_MOUSEBUTTONDOWN) {
                
                // record the position of the mouse            
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // convert to chessboard coordinates
                int row = 7 - mouseY / squareSize;
                int col = mouseX / squareSize;

                // convert to bitboard position
                int bitPos = row * 8 + col;
                U64 mask = 1ULL << bitPos;

                // check which piece has a corresponding bit set at that location.
                for(const auto& [type, bitboard] : chessBoard.getCurrentBoard()){
                    if(bitboard & mask){
                        selectedPiece = type;
                        selectedPieceX = col;
                        selectedPieceY = row;
                        break;
                    }

                }

                std::cout << "Selected " << pieceTypeToString(selectedPiece) << " at: " << row << ", " << col << endl; 

            }

            else if (windowEvent.type == SDL_MOUSEBUTTONUP) {
               
                // record the position of the mouse            
                int releaseX, releaseY;
                SDL_GetMouseState(&releaseX, &releaseY);

                // convert to chessboard coordinates
                int releaseRow = 7 - releaseY / squareSize;
                int releaseCol = releaseX / squareSize;

                // convert to bitboard position
                int releaseBitPos = releaseRow * 8 + releaseCol;
                U64 releaseMask = 1ULL << releaseBitPos;

                // need to check whether the move was valid
                U64 validMoves;

                if(selectedPiece == PieceType::WP){
                    
                    int fromBitPos = selectedPieceY * 8 + selectedPieceX;
                    validMoves = moveGenerator.generateWhitePawn(fromBitPos);
                
                }
                
                // if the bits allign, then we can move the piece
                if(validMoves & releaseMask){
                    
                    int fromBitPos = selectedPieceY * 8 + selectedPieceX;

                    // move the piece on the board
                    chessBoard.movePiece(selectedPiece, fromBitPos, releaseBitPos);

                    std::cout << "Moved " << pieceTypeToString(selectedPiece) << " to: " << releaseRow << ", " << releaseCol << endl; 

                } 

                else {
                    cout << "Invalid move" << endl;
                }

                // reset the selected piece type
                selectedPiece = PieceType::EMPTY;
            }

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