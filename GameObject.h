#pragma once
#include "Game.h"
#include <list>
#include <string>

class GameObject {
public:
	GameObject(std::string gameObjectTag,const char* textureSheet, SDL_Renderer* renderer, float xpos, float ypos, int spriteWidth, int spriteHeight);
	GameObject(GameObject* go);
	GameObject* clone() {
		return new GameObject(*this);
	}
	~GameObject();
	void Update();
	void Render();	
	bool Translate(float x, float y, std::list<SDL_Rect> colliders);
	float getX() { return xpos; }
	float getY() { return ypos; }
	void setX(float x) { this->xpos = x; }
	void setY(float y) { this->ypos = y; }
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
	void setFlipY(bool flipValue) {
		this->flipY = flipValue;
	}
	bool getFlipX() {
		return this->flipX;
	}
	bool getFlipY() {
		return this->flipY;
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
	void resetHits(){
		this->hit = 0;
	}
	int getNumberOfHits() {
		return this->hit;
	}
	void setDirectionX(int dir) {
		this->directionX = dir;
	}
	int getDirectionX() {
		return this->directionX;
	}
	void setScaleH(float scale) {
		this->scaleH = scale;
	}
	void setScaleW(float scale) {
		this->scaleW = scale;
	}
	bool TranslateX(float x, std::list<SDL_Rect> colliders);
	void resetAnimationCounter() {
		this->animationCounter = 0;
	}
	void setAnimationCounter(int frames) {
		this->animationCounter = frames;
	}
	void addToAnimationCounter(int frames) {
		this->animationCounter += frames;
	}
	int getAnimationCounter() {
		return this->animationCounter;
	}
	void setSpriteWidth(int value) {
		this->spriteWidth = value;
	}
	void setSpriteHeight(int value) {
		this->spriteHeight = value;
	}	
private:
	float xpos;
	float ypos;
	int spriteWidth;
	int spriteHeight;
	//got hit by the player
	int hit;
	
	float scaleW=1.0f;
	float scaleH=1.0f;

	int directionX = -1;//-1 is left and 1 is right
	
	//sprite options
	bool flipX=false;
	bool flipY=false;

	SDL_Texture* objTexture;
	SDL_Rect srcRect, destRect;
	SDL_Renderer* renderer;	
	float cameraXOffset=0;
	std::string gameObjectTag;

	int animationCounter=0;
};