#include "TextureManager.h"

SDL_Texture* TextureManager::LoadTexture(const char* fileName, SDL_Renderer* renderer) {
	SDL_Surface* tempSurface = IMG_Load(fileName);
	

	
	//removing the purple background
	Uint32 colorkey = SDL_MapRGB(tempSurface->format, 255, 0, 255);
	SDL_SetColorKey(tempSurface, SDL_TRUE, colorkey);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);

	SDL_FreeSurface(tempSurface);
	
	
	//std::printf("Failed to load %s\n", fileName);	

	return tex;
}