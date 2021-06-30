#pragma once
#include "Game.h"
#include <list>

class GameObject {
public:
	GameObject(const char* textureSheet, SDL_Renderer* renderer, float xpos, float ypos, int spriteWidth, int spriteHeight);
	~GameObject();
	void Update();
	void Render();
	void Translate(float x, float y, std::list<SDL_Rect> colliders);
	float getX() { return xpos; }
	float getY() { return ypos; }
	SDL_Rect getCollisionBox() {
		return destRect;
	}
	bool checkCollision(SDL_Rect otherObject);
private:
	float xpos;
	float ypos;
	int spriteWidth;
	int spriteHeight;

	SDL_Texture* objTexture;
	SDL_Rect srcRect, destRect;
	SDL_Renderer* renderer;
};