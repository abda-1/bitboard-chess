#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>
#include <UI.hpp>
#include <Board.hpp>
#include <MoveGenerator.hpp>
#include <AudioManager.hpp>

const int SQUARE_SIZE = 75;
const int BOARD_SIZE = 8;

bool initSDL();
SDL_Window* createWindow();
SDL_Renderer* createRenderer(SDL_Window* window);
void gameLoop(SDL_Renderer* renderer, Board& chessBoard, MoveGenerator& moveGenerator, UI& ui, AudioManager& audioManager);

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
    AudioManager audioManager;
    Board chessBoard(audioManager);
    MoveGenerator moveGenerator(chessBoard);
    UI ui(renderer, &chessBoard, SQUARE_SIZE);
    ui.loadImages();

    // Main Game
    gameLoop(renderer, chessBoard, moveGenerator, ui, audioManager);

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
void handleMouseDown (int mouseX, int mouseY, PieceType& selectedPiece, int& selectedPieceX, int& selectedPieceY, Board& chessBoard, bool& isWhiteTurn) {

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

        bool isWhitePiece = pieceTypeToString(selectedPiece)[0] == 'W';

        // Deselect invalid piece
        if ((isWhitePiece && !isWhiteTurn) || (!isWhitePiece && isWhiteTurn)) {
            selectedPiece = PieceType::EMPTY;
            return;
        }

        // Print
        std::cout << "Selected " << pieceTypeToString(selectedPiece) << " at: " << row << ", " << col << endl;        
        cout << "Selected Bit Pos: " << bitPos << endl;
        chessBoard.printU64(mask);
    }

}

// Handle mouse up events
void handleMouseUp (int releaseX, int releaseY, PieceType& selectedPiece, int& selectedPieceX, int& selectedPieceY, Board& chessBoard, MoveGenerator& moveGenerator, bool& isWhiteTurn, AudioManager& audioManager, U64& validMoves) {

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
    int selectedPiecePos = selectedPieceY * 8 + selectedPieceX;

    if (releaseBitPos == selectedPiecePos){
        return;
    }

    // if (validMoves == 0 && moveGenerator.isKingInCheck(isWhiteTurn)){
    //     audioManager.playSound(AudioType::CHECKMATE);
    //     return;
    // }


    // If move is valid, execute move
    if (validMoves & releaseMask) {

        
        chessBoard.executeMove(selectedPiece, selectedPiecePos, releaseBitPos);
        moveGenerator.updatePieces();

        // Switch turn only after valid move
        isWhiteTurn = !isWhiteTurn;

        // Print
        std::cout << "Moved " << pieceTypeToString(selectedPiece) << " to: " << releaseRow << ", " << releaseCol << endl;        
        cout << "Release Bit Pos: " << releaseBitPos << endl;
        chessBoard.printU64(1ULL << releaseBitPos);

        if (moveGenerator.isKingInCheck(isWhiteTurn)) audioManager.playSound(AudioType::CHECK);
    
    }

    else {
        std::cout << "Invalid move!" << std::endl;
    }

}



void gameLoop (SDL_Renderer* renderer, Board& chessBoard, MoveGenerator& moveGenerator, UI& ui, AudioManager& audioManager) {
    SDL_Event windowEvent;
    PieceType selectedPiece;
    int selectedPieceX, selectedPieceY;
    U64 validMoves = 0;
    bool isWhiteTurn = true;

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
                handleMouseDown(mouseX, mouseY, selectedPiece, selectedPieceX, selectedPieceY, chessBoard, isWhiteTurn);
                if (selectedPiece != PieceType::EMPTY) {
                    validMoves = moveGenerator.generatePieceValidMoves(selectedPiece, selectedPieceY * 8 + selectedPieceX);
                }

            }

            else if (windowEvent.type == SDL_MOUSEBUTTONUP) {
                       
                int releaseX, releaseY;
                SDL_GetMouseState(&releaseX, &releaseY);
                handleMouseUp(releaseX, releaseY, selectedPiece, selectedPieceX, selectedPieceY, chessBoard, moveGenerator, isWhiteTurn, audioManager, validMoves);
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

        // Check if the king is in check and draw a rectangle if it is
        if (moveGenerator.isKingInCheck(isWhiteTurn)) {

            U64 kingBoard = isWhiteTurn ? chessBoard.getCurrentBoard()[PieceType::WK] : chessBoard.getCurrentBoard()[PieceType::BK];
            int kingPosition = findLSBIndex(kingBoard);
            int kingRow = kingPosition / 8;
            int kingCol = kingPosition % 8;

            // Convert board position to pixel coordinates
            int x = kingCol * SQUARE_SIZE;
            int y = (7 - kingRow) * SQUARE_SIZE;

            // Set the color for the rectangle
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_Rect rect = {x, y, SQUARE_SIZE, SQUARE_SIZE};
            SDL_RenderDrawRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }
}