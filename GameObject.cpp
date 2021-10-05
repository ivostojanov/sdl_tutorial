#include "GameObject.h"
#include "TextureManager.h"

int frame = 0;

GameObject::GameObject(std::string gameObjectTag,const char* texturesheet, SDL_Renderer* renderer, float xpos, float ypos, int spriteWidth, int spriteHeight) {
	this->gameObjectTag = gameObjectTag;
	this->renderer = renderer;
	this->objTexture = TextureManager::LoadTexture(texturesheet, this->renderer);

	//setting the sprite width and height
	this->spriteHeight = spriteHeight;
	this->spriteWidth = spriteWidth;

	// default positions for the gameobject
	this->xpos = xpos;
	this->ypos = ypos;

	this->flipX = false;
	this->hit = 0;

	//set name tag
}

GameObject::GameObject(GameObject* go)
{
	this->gameObjectTag = go->gameObjectTag;
	this->renderer = go->renderer;
	this->objTexture = go->objTexture;
	this->spriteHeight = go->spriteHeight;
	this->spriteWidth = go->spriteWidth;
	this->xpos = go->xpos;
	this->ypos = go->ypos;
	this->flipX = go->flipX;
	this->hit = go->hit;
}

void GameObject::Update() {	

	//source values
	this->srcRect.h = spriteHeight;
	this->srcRect.w = spriteWidth;
	this->srcRect.x = 0;
	this->srcRect.y = 0;
	
	//destination values
	this->destRect.x = this->xpos+this->cameraXOffset;
	this->destRect.y = this->ypos;
	this->destRect.w = this->srcRect.w*scaleW;//you can scale the gameobject here, width
	this->destRect.h = this->srcRect.h*scaleH;//you can scale the gameobject here, height
}

void GameObject::Render() {	
	if (!flipX && !flipY) {
		SDL_RenderCopy(this->renderer, this->objTexture, &this->srcRect, &this->destRect);
	}
	
	if (flipY) {
		SDL_RenderCopyEx(this->renderer, this->objTexture, &this->srcRect, &this->destRect, NULL, NULL, SDL_FLIP_VERTICAL);
	}
	else if (flipX) {
		SDL_RenderCopyEx(this->renderer, this->objTexture, &this->srcRect, &this->destRect, NULL, NULL, SDL_FLIP_HORIZONTAL);
	}
	
}

bool GameObject::TranslateX(float x, std::list<SDL_Rect> colliders) {
	this->xpos += x;
	this->destRect.x = this->xpos + this->cameraXOffset;

	auto it = colliders.begin();
	for (; it != colliders.end(); ++it) {

		if (this->checkCollision(*it)) {
			this->xpos -= x;
			this->destRect.x = this->xpos + this->cameraXOffset;
			return true;
			break;
		}
	}
	return false;
}

bool GameObject::Translate(float x, float y, std::list<SDL_Rect> colliders)//this returns if you have hit anything on the y axis useful for the player movement
{
	if (x != 0) {
		this->xpos += x;
		this->destRect.x = this->xpos+this->cameraXOffset;

		auto it = colliders.begin();
		for (; it!=colliders.end(); ++it) {

			if (this->checkCollision(*it)) {
				this->xpos -= x;
				this->destRect.x = this->xpos+this->cameraXOffset;
				break;
			}
		}
		
	}
	
	if (y != 0) {
		this->ypos += y;
		this->destRect.y = this->ypos;

		auto it = colliders.begin();
		for (; it != colliders.end(); ++it) {
			if (this->checkCollision(*it)) {
				this->ypos -= y;
				this->destRect.y = this->ypos;
				return true;
				break;
			}
		}		
	}	

	return false;
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