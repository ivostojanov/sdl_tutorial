#include "Game.h"
#include "TextureManager.h"
#include "GameObject.h"
#include <list>

const int WIN_WIDTH=800, WIN_HEIGHT=600;

//Main player variables
GameObject* player;
float playerSpeed=0.25f;
float playerVelocity = 0.15f;
float playerStartSpeed = playerSpeed;

//Moving left or right
bool moveRightFlag = false, moveLeftFlag = false;

//grounded variable
bool grounded = true;

//animation counters
int animationFrameCounter = 0;

//falling variables
float playerFallSpeed = 3.0f;

// All these variables are related to jumping
bool playerHasJumped = false;
float jumpSpeed = 1.0f;
float jumpStartSpeed = jumpSpeed;
float jumpVelocity = 0.10f;

float jumpDistanceTraveled = 0.0f;
float jumpMaxDistance = 100.0f;

GameObject* luigiplayer;
std::list<GameObject*> ground;

//all the colliders in the level
std::list<SDL_Rect> colliders;

//movement animation
std::list<SDL_Texture*> playerMoveAnimation;
SDL_Texture* marioIdleSprite;
SDL_Texture* marioJumpSprite;

//frame counter
int frames = 0;

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

		this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
		if (this->renderer) {
			SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255); //setting the background to white
			std::cout << "Renderer created!" << std::endl;
		}
		this->isRunning = true;

		//after the init is succesful continue initializing things here

		//Instantiate the player gameobject	
		player = new GameObject("assets/images_png/mario.png", this->renderer, 32, WIN_HEIGHT-(32*3), 24, 32);
		//loading the move right animation
		marioIdleSprite = TextureManager::LoadTexture("assets/images_png/mario.png", this->renderer);
		marioJumpSprite = TextureManager::LoadTexture("assets/images_png/mario_jump.png", this->renderer);
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_png/mario_move0.png", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_png/mario_move1.png", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_png/mario_move2.png", this->renderer));

		luigiplayer = new GameObject("assets/images_png/luigi.png", this->renderer, 96, WIN_HEIGHT - (32 * 5), 24, 32);

		for (int i = 0; i < 20; i++) {
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

			//Getting all the colliders from the active objects FIX THIS
			colliders = std::list<SDL_Rect>();
			colliders.push_back(luigiplayer->getCollisionBox());
			for (GameObject* ground_tile : ground) {
				colliders.push_back(ground_tile->getCollisionBox());
			}

			if (event.key.keysym.sym == SDLK_a) {
				moveLeftFlag = true;

			}
			else if (event.key.keysym.sym == SDLK_d) {
				moveRightFlag = true;
			}
			else if (event.key.keysym.sym == SDLK_SPACE && !playerHasJumped && grounded) {
				playerHasJumped = true;
				grounded = false;
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}

		}else if (event.type == SDL_KEYUP) {
			playerSpeed = playerStartSpeed;
			switch (event.key.keysym.sym)
			{
				case SDLK_a:
					moveLeftFlag = false;
					break;
				case SDLK_d:
					moveRightFlag = false;
					break;
				default:
					break;
			}
		}
	}
}

void Game::setDeltaTime(float deltaTime)
{
	this->deltaTime = deltaTime;
}

void Game::Animate() {

	if (moveRightFlag) {
		player->setFlipX(false);		
		animationFrameCounter += 1;

		int animFrame = frames % 3;

		auto it = playerMoveAnimation.begin();
		std::advance(it, animFrame);		
		
		player->SetCurrentTexture(*it);
	}
	else if (moveLeftFlag) {
		player->setFlipX(true);
		int animFrame = frames % 3;
		for (SDL_Texture* texture : playerMoveAnimation) {
			if (animFrame == 0) {
				player->SetCurrentTexture(texture);
				break;
			}
			animFrame--;
		}
	}
	else {
		player->SetCurrentTexture(marioIdleSprite);
	}

	if (playerHasJumped || !grounded) {
		player->SetCurrentTexture(marioJumpSprite);
	}
	

	frames++;
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
	
	if (colliders.size() != 0 && !playerHasJumped) {
		grounded = player->Translate(0, (jumpSpeed*playerFallSpeed), colliders);
	}

	if(playerHasJumped){
		bool jumpFlag = player->Translate(0, (-1)*jumpSpeed, colliders);//constantly going upwards
		jumpDistanceTraveled += jumpSpeed; //calculate the distance we have jumped
		jumpSpeed += jumpVelocity;//increasing our jump speed
		playerHasJumped = !jumpFlag;//if we have hit an object stop the jump or if we reached our jumping limit
		if (jumpDistanceTraveled > jumpMaxDistance) {
			jumpDistanceTraveled = 0.0f;
			playerHasJumped = false;
		}
	}
	else {
		jumpSpeed = jumpStartSpeed;
	}

	if (moveLeftFlag) {
		player->Translate(-1 * playerSpeed, 0, colliders);
		playerSpeed += playerVelocity;
	}

	if (moveRightFlag) {
		player->Translate(1 * playerSpeed, 0, colliders);
		playerSpeed += playerVelocity;
	}	

	this->Animate();
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