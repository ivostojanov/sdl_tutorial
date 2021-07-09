#include <SDL.h>
#include <iostream>
#include <string>
#include <SDL_image.h>

#pragma once
class Game
{
	public:
		Game(const char* title, int xpos, int ypos, bool fullscreen);		
		~Game();
		void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
		void update();
		void render();
		void clean();
		void handleEvents();

		bool running() { return isRunning; };
		void setDeltaTime(float deltaTime);

		void Animate();
		//set the camera over an object - preferably the player
		void setCamera();
		float getDeltaTime() {
			return this->deltaTime / 1000;
		}

	private:	
		double cnt;
		bool isRunning;
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Surface *loadSurface(std::string path);
		float deltaTime;//in seconds
};

