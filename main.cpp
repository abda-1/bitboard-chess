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
    MoveGenerator moveGenerator(chessBoard);

    UI ui(renderer, &chessBoard, squareSize);
    ui.loadImages();

    // initialise main game loop
    SDL_Event windowEvent;
    PieceType selectedPiece;
    PieceType capturedPiece;
    int selectedPieceX, selectedPieceY;

    while(true){
        
        // event handling
        if(SDL_PollEvent(&windowEvent)){
            
            // quit condition
            if(SDL_QUIT == windowEvent.type){
                break;
            }

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

                // make sure that a valid piece was selected
                if(selectedPiece != PieceType::EMPTY){
                    std::cout << "Selected " << pieceTypeToString(selectedPiece) << " at: " << row << ", " << col << endl;
                    cout << "Selected Bit Pos: " << bitPos << endl;
                    chessBoard.printU64(mask);
                }
                
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

                int fromBitPos = selectedPieceY * 8 + selectedPieceX;

                // determine which piece was selected, and if the square was empty, do nothing
                if (selectedPiece == PieceType::EMPTY) {
                    continue;
                }

                else if (selectedPiece == PieceType::WP) {
                    validMoves = moveGenerator.generateWhitePawn(fromBitPos);
                }
                
                else if (selectedPiece == PieceType::BP) {
                    validMoves = moveGenerator.generateBlackPawn(fromBitPos);
                }

                else if (selectedPiece == PieceType::WN) {
                    validMoves = moveGenerator.generateWhiteKnight(fromBitPos);
                }

                else if (selectedPiece == PieceType::BN) {
                    validMoves = moveGenerator.generateBlackKnight(fromBitPos);
                }

                else if (selectedPiece == PieceType::WR) {
                    validMoves = moveGenerator.generateWhiteRook(fromBitPos);
                }

                else if (selectedPiece == PieceType::BR) {
                    validMoves = moveGenerator.generateBlackRook(fromBitPos);
                }

                else if (selectedPiece == PieceType::WB) {
                    validMoves = moveGenerator.generateWhiteBishop(fromBitPos);
                }

                else if (selectedPiece == PieceType::BB) {
                    validMoves = moveGenerator.generateBlackBishop(fromBitPos);
                }

                else if (selectedPiece == PieceType::WQ) {
                    validMoves = moveGenerator.generateWhiteQueen(fromBitPos);
                }

                else if (selectedPiece == PieceType::BQ) {
                    validMoves = moveGenerator.generateBlackQueen(fromBitPos);
                }

                else if (selectedPiece == PieceType::WK) {
                    validMoves = moveGenerator.generateWhiteKing(fromBitPos);
                }

                else if (selectedPiece == PieceType::BK) {
                    validMoves = moveGenerator.generateBlackKing(fromBitPos);
                }
                
                // if the bits allign, then we can move the piece
                if(validMoves & releaseMask){
                    PieceType capturedPiece = PieceType::EMPTY;
                    
                    for(const auto& [type, bitboard] : chessBoard.getCurrentBoard()){
                        if((bitboard & releaseMask) && (type != selectedPiece)){
                            capturedPiece = type;
                            break;
                        }
                    }

                    // check whether the move was a capture -> capturedPiece needs to be opponent piece. 
                    if(capturedPiece != PieceType::EMPTY && chessBoard.isOpponentPiece(selectedPiece, capturedPiece)){

                        chessBoard.capturePiece(capturedPiece, releaseBitPos);
                        cout << pieceTypeToString(selectedPiece) << " captured " << pieceTypeToString(capturedPiece) << "!" << endl;
                        
                        chessBoard.movePiece(selectedPiece, fromBitPos, releaseBitPos);
                        std::cout << "Moved " << pieceTypeToString(selectedPiece) << " to: " << releaseRow << ", " << releaseCol << endl;
                        
                        cout << "Release Bit Pos: " << releaseBitPos << endl;
                        chessBoard.printU64(releaseMask);

                    }

                    // move was not a capture move
                    else if (capturedPiece == PieceType::EMPTY) {

                        chessBoard.movePiece(selectedPiece, fromBitPos, releaseBitPos);
                        std::cout << "Moved " << pieceTypeToString(selectedPiece) << " to: " << releaseRow << ", " << releaseCol << endl;
                        
                        chessBoard.printU64(releaseMask);

                    }
                    
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