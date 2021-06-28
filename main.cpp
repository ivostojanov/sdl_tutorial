#include "Game.h"

Game *game = NULL;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int args, char* argv[]) {	
	game = new Game("Super Mario Bros - Level 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, false);	

	while (game->running()) {
		game->handleEvents();
		game->update();
		game->render();
	}
	game->clean();	
	return 0;
}