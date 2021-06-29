#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject(const char* texturesheet, SDL_Renderer* renderer, int xpos, int ypos, int spriteWidth, int spriteHeight) {
	this->renderer = renderer;
	this->objTexture = TextureManager::LoadTexture(texturesheet, this->renderer);

	//setting the sprite width and height
	this->spriteHeight = spriteHeight;
	this->spriteWidth = spriteWidth;

	// default positions for the gameobject
	this->xpos = xpos;
	this->ypos = ypos;
}

void GameObject::Update() {
	this->xpos++;
	this->ypos++;

	//source values
	this->srcRect.h = spriteHeight;
	this->srcRect.w = spriteWidth;
	this->srcRect.x = 0;
	this->srcRect.y = 0;
	
	//destination values
	this->destRect.x = this->xpos;
	this->destRect.y = this->ypos;
	this->destRect.w = this->srcRect.w;//you can scale the gameobject here, width
	this->destRect.h = this->srcRect.h;//you can scale the gameobject here, height
}

void GameObject::Render() {
	SDL_RenderCopy(this->renderer, this->objTexture, &this->srcRect, &this->destRect);
}