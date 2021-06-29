#include "Game.h"

Game *game = NULL;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int FPS = 60;
const int frameDelay = 1000 / FPS;

int main(int args, char* argv[]) {	
	game = new Game("Super Mario Bros - Level 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, false);	

	Uint32 frameStart;
	int deltaTime;

	while (game->running()) {
		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();
		
		//capping the framerate to 60
		deltaTime = SDL_GetTicks() - frameStart;
		if (deltaTime < frameDelay) {
			SDL_Delay(frameDelay - deltaTime);
		}
	}
	game->clean();	
	return 0;
}