#include "Game.h"
#include "TextureManager.h"
#include "GameObject.h"
#include <list>

const int WIN_WIDTH=800, WIN_HEIGHT=600;

//Main player variables
GameObject* player;
float playerSpeed=5.0f;
float playerVelocity = 0.15f;
float playerStartSpeed = 5.0f;

bool playerHasJumped = false;
float jumpDistance = 0.0f;
float jumpMaxDistance = 32 * 2;
float jumpSpeed = 10.0f;

GameObject* luigiplayer;
std::list<GameObject*> ground;

Game::Game(const char* title, int xpos, int ypos, bool fullscreen)
{
	//Default values
	this->isRunning = false;
	this->window = NULL;
	this->renderer = NULL;

	//Initializing the window
	this->init(title, xpos, ypos, WIN_WIDTH, WIN_HEIGHT, fullscreen);
	this->cnt = 0;	
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

		//Instantiate the player gameobject	
		player = new GameObject("assets/images_png/mario.png", this->renderer, 32, WIN_HEIGHT-(32*3), 24, 32);
		luigiplayer = new GameObject("assets/images_png/luigi.png", this->renderer, 96, WIN_HEIGHT - (32 * 5), 24, 32);

		for (int i = 0; i < 50; i++) {
			GameObject* temp;

			temp = new GameObject("assets/images/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject("assets/images/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

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

		if (event.type == SDL_KEYDOWN) {	
			
			//Getting all the colliders from the active objects
			std::list<SDL_Rect> colliders;
			colliders.push_back(luigiplayer->getCollisionBox());
			for (GameObject* ground_tile : ground) {
				colliders.push_back(ground_tile->getCollisionBox());
			}

			if (event.key.keysym.sym == SDLK_a) {								
				player->Translate(-1 * playerSpeed, 0, colliders);
				playerSpeed += playerVelocity;

			}else if(event.key.keysym.sym == SDLK_d) {				
				player->Translate(1 * playerSpeed, 0, colliders);
				playerSpeed += playerVelocity;
			}
			else if (event.key.keysym.sym == SDLK_SPACE) {
				playerHasJumped = true;
			}			
		}	
		if (event.type == SDL_KEYUP) {
			playerSpeed = playerStartSpeed;
		}
	}
}

void Game::setDeltaTime(float deltaTime)
{
	this->deltaTime = deltaTime;
}

void Game::update()
{
	//update positions here	
	player->Update(); //updating the player values
	luigiplayer->Update();
	
	for(GameObject* tile : ground)
	{
		tile->Update();
	}

	/* FIX THIS JUMP
	if (playerHasJumped == true) {
		float jumpValue = -0.01f * this->deltaTime * jumpSpeed;
		player->Translate(0, jumpValue);		
		jumpDistance += jumpValue;

		if (jumpDistance*(-1) >= jumpMaxDistance) {			
			playerHasJumped = false;
		}		
	}
	else if (playerHasJumped == false && jumpDistance >= jumpMaxDistance) {
		float fallValue = 0.01f * this->deltaTime;
		player->Translate(0, fallValue);
	}*/
}

void Game::render()
{	
	SDL_RenderClear(this->renderer);

	//Rendering happens here
	player->Render();//rendering the player
	luigiplayer->Render();

	for (GameObject* tile : ground)
	{
		tile->Render();
	}

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