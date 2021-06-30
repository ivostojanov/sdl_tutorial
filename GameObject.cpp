#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject(const char* texturesheet, SDL_Renderer* renderer, float xpos, float ypos, int spriteWidth, int spriteHeight) {
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

void GameObject::Translate(float x, float y, std::list<SDL_Rect> colliders)
{
	this->xpos += x;
	this->destRect.x = this->xpos;
	for (SDL_Rect collider:colliders)
	{
		if (this->checkCollision(collider)) {
			this->xpos -= x;
			this->destRect.x = this->xpos;
			break;
		}
	}
	
	this->ypos += y;
	this->destRect.y = this->ypos;
	for (SDL_Rect collider : colliders)
	{
		if (this->checkCollision(collider)) {
			this->ypos -= y;
			this->destRect.y = this->ypos;
			break;
		}
	}
}

bool GameObject::checkCollision(SDL_Rect otherObject)
{
	bool collisionCheck1 = (otherObject.x+otherObject.w)>destRect.x;
	bool collisionCheck2 = (destRect.x + destRect.w) > otherObject.x;
	bool collisionCheck3 = (otherObject.y + otherObject.h) > destRect.y;
	bool collisionCheck4 = (destRect.y + destRect.h) > otherObject.y;

	if (collisionCheck1 && collisionCheck2 && collisionCheck3 && collisionCheck4) {
		return true;
	}

	return false;
}
