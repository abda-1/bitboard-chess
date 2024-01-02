#include <SDL.h>
#include <iostream>
#include <UI.hpp>
#include <Board.hpp>
#include <MoveGenerator.hpp>
#include <Game.hpp>

const int SQUARE_SIZE = 75;
const int BOARD_SIZE = 8;

bool initSDL();
SDL_Window* createWindow();
SDL_Renderer* createRenderer(SDL_Window* window);
void gameLoop(SDL_Renderer* renderer, Board& chessBoard, MoveGenerator& moveGenerator, Game& game, UI& ui);

int main(int argc, char *argv[]){

    // Initialise SDL
    if (!initSDL()) return -1;
    
    // Create Window
    SDL_Window* window = createWindow();
    if (!window) return -1;

    // Create Renderer
    SDL_Renderer* renderer = createRenderer(window);
    if (!renderer) return -1;

    // Initialise Game
    Board chessBoard;
    MoveGenerator moveGenerator(chessBoard);   // think abt this does it need to be reference
    Game game;
    UI ui(renderer, &chessBoard, SQUARE_SIZE);
    ui.loadImages();

    // Main Game
    gameLoop(renderer, chessBoard, moveGenerator, game, ui);

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;

}

bool initSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initalise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

SDL_Window* createWindow() {
    SDL_Window* window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SQUARE_SIZE*8, SQUARE_SIZE*8, SDL_WINDOW_ALLOW_HIGHDPI);
    if( window == NULL){
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return NULL;
    }
    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    return renderer;
}

// Handle mouse down events
void handleMouseDown (int mouseX, int mouseY, PieceType& selectedPiece, int& selectedPieceX, int& selectedPieceY, Board& chessBoard) {

    // Convert mouse coordinates to bitboard position and chessboard coordinates
    int row = 7 - mouseY / SQUARE_SIZE;
    int col = mouseX / SQUARE_SIZE;
    int bitPos = row * 8 + col;
    U64 mask = 1ULL << bitPos;

    // Check which piece has a corresponding bit set at that location.
    for(const auto& [type, bitboard] : chessBoard.getCurrentBoard()){
        if(bitboard & mask){
            selectedPiece = type;
            selectedPieceX = col;
            selectedPieceY = row;
            break;
        }
    }

    // Log the selected piece (to ensure valid piece was selected)
    if(selectedPiece != PieceType::EMPTY){
        std::cout << "Selected " << pieceTypeToString(selectedPiece) << " at: " << row << ", " << col << endl;
        
        // Print
        cout << "Selected Bit Pos: " << bitPos << endl;
        chessBoard.printU64(mask);
    }

}

// Handle mouse up events
void handleMouseUp (int releaseX, int releaseY, PieceType& selectedPiece, int& selectedPieceX, int& selectedPieceY, Board& chessBoard, MoveGenerator& moveGenerator) {

    // Convert mouse coordinates to bitboard position and chessboard coordinates
    int releaseRow = 7 - releaseY / SQUARE_SIZE;
    int releaseCol = releaseX / SQUARE_SIZE;
    int releaseBitPos = releaseRow * 8 + releaseCol;
    U64 releaseMask = 1ULL << releaseBitPos;

    // Do nothing if initially clicked on a square that was empty
    if (selectedPiece == PieceType::EMPTY) {
        return;
    }

    // Generate all valid moves on the location of the piece
    int selectedPiecePosition = selectedPieceY * 8 + selectedPieceX;
    U64 validMoves = moveGenerator.generateMoves(selectedPiece, selectedPiecePosition);

    // If move is valid, execute move
    if (validMoves & releaseMask) {
        chessBoard.executeMove(selectedPiece, selectedPieceY * 8 + selectedPieceX, releaseBitPos);
        
        // Print
        cout << "Release Bit Pos: " << releaseBitPos << endl;
        chessBoard.printU64(1ULL << releaseBitPos);

    } else {
        std::cout << "Invalid move!" << std::endl;
    }

}



void gameLoop (SDL_Renderer* renderer, Board& chessBoard, MoveGenerator& moveGenerator, Game& game, UI& ui) {
    SDL_Event windowEvent;
    PieceType selectedPiece;
    int selectedPieceX, selectedPieceY;
    U64 validMoves = 0;

    while(true){
        
        // event handling
        if(SDL_PollEvent(&windowEvent)){
            
            // quit condition
            if(SDL_QUIT == windowEvent.type){
                break;
            }

            else if (windowEvent.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                handleMouseDown(mouseX, mouseY, selectedPiece, selectedPieceX, selectedPieceY, chessBoard);
                if (selectedPiece != PieceType::EMPTY) {
                    validMoves = moveGenerator.generateMoves(selectedPiece, selectedPieceY * 8 + selectedPieceX);
                }

            }

            else if (windowEvent.type == SDL_MOUSEBUTTONUP) {
                       
                int releaseX, releaseY;
                SDL_GetMouseState(&releaseX, &releaseY);
                handleMouseUp(releaseX, releaseY, selectedPiece, selectedPieceX, selectedPieceY, chessBoard, moveGenerator);
                selectedPiece = PieceType::EMPTY;
                validMoves = 0;

            }

        }

        // Render the board after each event
        SDL_RenderClear(renderer);
        ui.drawChessboard();
        ui.drawPieces();
        if (selectedPiece != PieceType::EMPTY) {
            ui.drawValidMoves(validMoves);
        }
        SDL_RenderPresent(renderer);
    }
}