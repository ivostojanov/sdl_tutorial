#pragma once
#include "Game.h"
#include <list>
#include <string>

class GameObject {
public:
	GameObject(std::string gameObjectTag,const char* textureSheet, SDL_Renderer* renderer, float xpos, float ypos, int spriteWidth, int spriteHeight);
	~GameObject();
	void Update();
	void Render();	
	bool Translate(float x, float y, std::list<SDL_Rect> colliders);
	float getX() { return xpos; }
	float getY() { return ypos; }
	SDL_Rect getCollisionBox() {
		return destRect;
	}
	void SetCurrentTexture(SDL_Texture* texture) {
		this->objTexture = texture;
	}
	bool checkCollision(SDL_Rect otherObject);
	void setFlipX(bool flipValue) {
		this->flipX = flipValue;
	}	
	void setCameraOffset(float x) {
		cameraXOffset = x;
	}
	std::string getTag() {
		return this->gameObjectTag;
	}
	void incrementHit() {
		this->hit++;
	}
	int getNumberOfHits() {
		return this->hit;
	}
private:
	float xpos;
	float ypos;
	int spriteWidth;
	int spriteHeight;
	//got hit by the player
	int hit = 0;
	
	//sprite options
	bool flipX;

	SDL_Texture* objTexture;
	SDL_Rect srcRect, destRect;
	SDL_Renderer* renderer;	
	float cameraXOffset=0;
	std::string gameObjectTag;
};