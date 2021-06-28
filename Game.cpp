#include "Game.h"

SDL_Texture* playerTex;
SDL_Rect* srcR, destR;

int WIN_WIDTH, WIN_HEIGHT;

Game::Game(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	this->isRunning = false;
	this->window = NULL;
	this->renderer = NULL;
	this->init(title, xpos, ypos, width, height, fullscreen);
	this->cnt = 0;

	WIN_WIDTH = width;
	WIN_HEIGHT = height;
}

Game::~Game()
{
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	//Setting the fullscreen flag for the window creation
	int fullscreenFlag = 0;
	if (fullscreen) {
		fullscreenFlag = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING)==0) {
		std::cout << "SDL initialized succesfully!" << std::endl;
		
		this->window = SDL_CreateWindow(title, xpos, ypos, width, height, fullscreenFlag);
		if (this->window) {
			std::cout << "Window has been created" << std::endl;

			//Setting the icon of the window
			SDL_Surface* iconSurface = this->loadSurface("assets/images_png/icon.png");			
			if (iconSurface != NULL) {
				SDL_SetWindowIcon(this->window, iconSurface);
				std::cout << "Icon has been succesfully loaded and placed!" << std::endl;
			}
		}

		this->renderer = SDL_CreateRenderer(this->window, -1, 0);
		if (this->renderer) {
			SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255); //setting the background to white
			std::cout << "Renderer created!" << std::endl;
		}
		this->isRunning = true;

		//after the init is succesful continue initializing things here

		//Loading the main player texture
		SDL_Surface* tmpSurface = loadSurface("assets/images_png/mario.png");
		playerTex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}
	else {
		this->isRunning = false;
	}
	
}

void Game::handleEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				default:
					break;
			}
		}
	}
}

void Game::update()
{
	//update positions here	
	destR.w = 24;
	destR.h = 32;
	destR.y = WIN_HEIGHT - destR.h;
	destR.x = this->cnt;

	this->cnt+=0.1;
}

void Game::render()
{	
	SDL_RenderClear(this->renderer);

	//Rendering happens here
	SDL_RenderCopy(this->renderer, playerTex, NULL, &destR);

	SDL_RenderPresent(this->renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(this->window);
	SDL_DestroyRenderer(this->renderer);
	SDL_Quit();
	std::cout << "Game cleaned" << std::endl;
}

SDL_Surface* Game::loadSurface(std::string path) {
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		std::printf("image(%s) failed to load\n", path.c_str());
	}
	else {
		std::printf("image(%s) is loaded succesfully!!!\n", path.c_str());
	}
		
	return loadedSurface;
}