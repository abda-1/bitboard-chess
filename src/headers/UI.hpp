#ifndef UI_HPP
#define UI_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <PieceType.hpp>
#include <TextureManager.hpp>
#include <Board.hpp>
#include <BitOperations.hpp>
#include <unordered_map>

using namespace std;

class UI{

    private:
        int SQUARE_SIZE;
        int BOARD_SIZE;
        
        TextureManager textureManager;
        SDL_Renderer* renderer;
        Board* board;

        void setSquareSize(int SQ_SIZE);

    public:
        UI(SDL_Renderer* r, Board* b, int size) : renderer(r), textureManager(r), board(b), SQUARE_SIZE(size), BOARD_SIZE(size * 8) {};

        int getBoardSize();
        
        void drawChessboard();
        void drawPieces();
        void loadImages();

        void drawValidMoves(U64 validMoves);
        void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);

        // Copy constructor and copy assignment operators should not be allowed
        UI(const UI&) = delete;
        UI& operator=(const UI&) = delete;
        
};

void UI::drawValidMoves (U64 validMoves) {
    SDL_SetRenderDrawColor (renderer, 209, 213, 183, SDL_ALPHA_OPAQUE);
    int radius = SQUARE_SIZE/6;
    while (validMoves) {

        int index = findLSBIndex(validMoves);
        if (index == -1) break;

        // Clear the bit that was just found
        validMoves &= ~(1ULL << index);
    
        int row = index / 8;
        int col = index % 8;
        int x = col * SQUARE_SIZE + SQUARE_SIZE/2;
        int y = (7 - row) * SQUARE_SIZE + SQUARE_SIZE/2;

        drawFilledCircle(renderer, x, y, radius);
    
    }

}

void UI::drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void UI::setSquareSize(int SQ_SIZE){
    SQUARE_SIZE = SQ_SIZE;
    BOARD_SIZE = SQ_SIZE * 8;
}

int UI::getBoardSize(){
    return BOARD_SIZE;
}

void UI::loadImages(){

    // Error handling has not been placed in yet. 

    // Black pieces
    textureManager.loadTexture(PieceType::BP, "src/assets/textures/bp.png");
    textureManager.loadTexture(PieceType::BR, "src/assets/textures/br.png");
    textureManager.loadTexture(PieceType::BB, "src/assets/textures/bb.png");
    textureManager.loadTexture(PieceType::BN, "src/assets/textures/bn.png");
    textureManager.loadTexture(PieceType::BQ, "src/assets/textures/bq.png");
    textureManager.loadTexture(PieceType::BK, "src/assets/textures/bk.png");
    
    // White pieces
    textureManager.loadTexture(PieceType::WP, "src/assets/textures/wp.png");
    textureManager.loadTexture(PieceType::WR, "src/assets/textures/wr.png");
    textureManager.loadTexture(PieceType::WB, "src/assets/textures/wb.png");
    textureManager.loadTexture(PieceType::WN, "src/assets/textures/wn.png");
    textureManager.loadTexture(PieceType::WQ, "src/assets/textures/wq.png");
    textureManager.loadTexture(PieceType::WK, "src/assets/textures/wk.png");

}

void UI::drawChessboard(){

    SDL_Rect square;

    square.w = SQUARE_SIZE;
    square.h = SQUARE_SIZE;

    // set up board to be alternating colours
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){

            if ((i+j) % 2 == 0) SDL_SetRenderDrawColor(renderer,233,237,204,255);  // Colour 1
            else                SDL_SetRenderDrawColor(renderer,119,153,84,255);   // Colour 2

            // X-coordinate of square
            square.x = j * SQUARE_SIZE;                     

            // Y-coordinate of square
            square.y = i * SQUARE_SIZE;                     

            SDL_RenderFillRect(renderer, &square);
        }
    }

}

void UI::drawPieces(){

    // Recall that piece textures are stored in the textures map in textureManager.

    for(const auto& [type, bb] : board->getCurrentBoard()){

        U64 mask = 1ULL;
        for(int i = 0; i < 64; i++, mask <<= 1){
            
            // If there is an active bit at the current location, a piece exists!
            if(bb & mask){

                // Calculate coordinates in terms of row and col, and then convert to pixel coordinates.

                int row = 7 - (i / 8);
                int col = i % 8;

                int x = col * SQUARE_SIZE;
                int y = row * SQUARE_SIZE;

                // Now obtain the texture for this piece and then draw on screen
                SDL_Texture* texture = textureManager.getTexture(type);
                SDL_Rect destRect = {x, y, SQUARE_SIZE, SQUARE_SIZE};

                if(texture){
                    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
                }

            }
        
        }
    }

}

#endif  // UI_HPP