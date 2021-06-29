#pragma once
#include "Game.h"

class GameObject {
public:
	GameObject(const char* textureSheet, SDL_Renderer* renderer, int xpos, int ypos, int spriteWidth, int spriteHeight);
	~GameObject();
	void Update();
	void Render();
private:
	int xpos;
	int ypos;
	int spriteWidth;
	int spriteHeight;

	SDL_Texture* objTexture;
	SDL_Rect srcRect, destRect;
	SDL_Renderer* renderer;
};