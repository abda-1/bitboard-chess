#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <PieceType.hpp>
#include <string>
#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;

class TextureManager{

    private:
        // Set up map for piecetype and corresponding (current) texture
        std::unordered_map<PieceType, SDL_Texture*> textures;
        SDL_Renderer* renderer;

    public:
        TextureManager(SDL_Renderer* r);
        ~TextureManager();

        SDL_Texture* loadTexture(PieceType type, const string path);
        SDL_Texture* getTexture(PieceType type);
        void clearTexture(PieceType type);

        // Copy constructor and copy assignment operators should not be allowed
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

};

TextureManager::TextureManager(SDL_Renderer* r){
    renderer = r;
}

TextureManager::~TextureManager(){
    for(auto& texture : textures){
        SDL_DestroyTexture(texture.second);
    }
}

SDL_Texture* TextureManager::loadTexture(PieceType type, const string path){

    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

    if(texture == NULL){
        std::cerr << "Unable to load image at " << path << "! SDL_Image Error: " << IMG_GetError() << endl;
        return nullptr;
    }

    textures[type] = texture;
    return texture;

}

SDL_Texture* TextureManager::getTexture(PieceType type){

    auto it = textures.find(type);
    if(it != textures.end()){
        return it->second;
    }

    return nullptr;

}

void TextureManager::clearTexture(PieceType type){

    auto it = textures.find(type);
    if(it != textures.end()){
        SDL_DestroyTexture(it->second);
        textures.erase(it);
    }

}

#endif // TEXTUREMANAGER_H