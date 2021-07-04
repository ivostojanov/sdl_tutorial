#include "Game.h"

Game *game = NULL;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int FPS = 60;
const float frameDelay = 1000.0f / FPS;

int main(int args, char* argv[]) {	
	game = new Game("Super Mario Bros - Level 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, false);	

	Uint32 frameStart;
	float deltaTime=0;	

	while (game->running()) {
		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();
		
		
		//capping the framerate to 60
		deltaTime = (SDL_GetTicks() - frameStart);				
		//game->setDeltaTime(deltaTime + (frameDelay - deltaTime));

		//removing frame capping
		if (deltaTime < frameDelay) {
			//there is an issue with SDL_Delay
			//SDL_Delay(frameDelay - deltaTime);
		}	
		
	}
	game->clean();	
	return 0;
}