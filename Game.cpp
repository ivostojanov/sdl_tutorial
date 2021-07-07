#include "Game.h"
#include "TextureManager.h"
#include "GameObject.h"
#include <list>

const int WIN_WIDTH=640, WIN_HEIGHT=480;

//Main player variables
GameObject* player;
float playerSpeed=2.0f;
float playerVelocity = 0.0f;
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
float jumpSpeed = 4.0f;
float jumpStartSpeed = jumpSpeed;
float jumpVelocity = 0.01f;

float jumpDistanceTraveled = 0.0f;
float jumpMaxDistance = 150.0f;

std::list<GameObject*> ground;
std::list<GameObject*> magicBoxes;
std::list<GameObject*> brickedWalls;
std::list<GameObject*> decor;

//all the colliders in the level
std::list<SDL_Rect> colliders;

//movement animation
std::list<SDL_Texture*> playerMoveAnimation;
SDL_Texture* marioIdleSprite;
SDL_Texture* marioJumpSprite;

//magic box animation
std::list<SDL_Texture*> magicBoxIdleAnimation;
int magicBoxAnimationFrameCounter = 0;
int moveAnimationFrameCounter = 0;
//SDL_Texture* blockUsedAnimation;

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
			SDL_Surface* iconSurface = this->loadSurface("assets/images_custom/mario/mario.bmp");	
			//removing the purple background
			Uint32 colorkey = SDL_MapRGB(iconSurface->format, 255, 0, 255);
			SDL_SetColorKey(iconSurface, SDL_TRUE, colorkey);
			if (iconSurface != NULL) {
				SDL_SetWindowIcon(this->window, iconSurface);
				std::cout << "Icon has been succesfully loaded and placed!" << std::endl;
			}
		}

		this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED); //|SDL_RENDERER_PRESENTVSYNC
		if (this->renderer) {
			SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255); //setting the background to white
			std::cout << "Renderer created!" << std::endl;
		}
		this->isRunning = true;

		//after the init is succesful continue initializing things here

		//Instantiate the player gameobject	
		player = new GameObject("assets/images_custom/ico.bmp", this->renderer, 32*2, WIN_HEIGHT-(32*3), 24, 32);
		//loading the move right animation
		marioIdleSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario.bmp", this->renderer);
		marioJumpSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario_jump.bmp", this->renderer);
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move0.bmp", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move1.bmp", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move2.bmp", this->renderer));

		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_0.bmp", this->renderer));
		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_1.bmp", this->renderer));
		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_2.bmp", this->renderer));

		//the ground
		for (int i = 0; i < 40; i++) {
			GameObject* temp;
			temp = new GameObject("assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject("assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

		//starting obstacles
		magicBoxes.push_back(new GameObject("assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 13, WIN_HEIGHT- 6 * 32, 32, 32));

		brickedWalls.push_back(new GameObject("assets/images_custom/blocks/brickred.bmp", this->renderer, 32*17, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject("assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 18, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedWalls.push_back(new GameObject("assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 19, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject("assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 20, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedWalls.push_back(new GameObject("assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 21, WIN_HEIGHT - 6 * 32, 32, 32));

		magicBoxes.push_back(new GameObject("assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (6+4) * 32, 32, 32));
		
		//--------------Scene decor--------------
		//First bush
		decor.push_back(new GameObject("assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 0, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 1, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * 2, WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32*1, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32*2, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32*3, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 3, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 4, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * 2, WIN_HEIGHT - (32 * 4), 32, 32));

		//second grass
		int decor_bush = 8;
		decor.push_back(new GameObject("assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//third bush		
		decor.push_back(new GameObject("assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 13, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * 14, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * 14, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 15, WIN_HEIGHT - (32 * 3), 32, 32));

		//fourth grass
		decor.push_back(new GameObject("assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * 20, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * 21, WIN_HEIGHT - (32 * 3), 32, 32));

		//clouds
		int cloud_position_x = 6;
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 4), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 3), 32, 32));

		cloud_position_x = 17;
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));
		decor.push_back(new GameObject("assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));
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
			
			for (GameObject* magicBox : magicBoxes) {
				colliders.push_back(magicBox->getCollisionBox());
			}

			for (GameObject* brickedWall : brickedWalls) {
				colliders.push_back(brickedWall->getCollisionBox());
			}

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
				jumpDistanceTraveled = 0;
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
		
		int animFrame = 0;
		if (moveAnimationFrameCounter <= 10) {
			animFrame = 2;
		}
		else if (moveAnimationFrameCounter <= 20) {
			animFrame = 1;
		}
		else if (moveAnimationFrameCounter <= 30) {
			animFrame = 0;
		}
		else {
			moveAnimationFrameCounter = 0;
		}	

		auto it = playerMoveAnimation.begin();
		std::advance(it, animFrame);		
		
		player->SetCurrentTexture(*it);
	}
	else if (moveLeftFlag) {
		player->setFlipX(true);
		int animFrame = 0;

		if (moveAnimationFrameCounter <= 10) {
			animFrame = 0;
		}
		else if (moveAnimationFrameCounter <= 20) {
			animFrame = 1;
		}
		else if (moveAnimationFrameCounter <= 30) {
			animFrame = 2;
		}
		else {
			moveAnimationFrameCounter = 0;
		}

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
	
	for (GameObject* magicBox : magicBoxes) {
		int animFrame = 0;
		if (magicBoxAnimationFrameCounter <= 15) {
			animFrame = 0;
		}
		else if (magicBoxAnimationFrameCounter <= 30) {
			animFrame = 1;
		}
		else if (magicBoxAnimationFrameCounter <= 45) {
			animFrame = 2;
		}
		else {
			magicBoxAnimationFrameCounter = 0;
		}

		auto it = magicBoxIdleAnimation.begin();
		std::advance(it, animFrame);
		magicBox->SetCurrentTexture(*it);		
	}
	
	magicBoxAnimationFrameCounter++;
	moveAnimationFrameCounter++;
	frames++;
}

void Game::update()
{
	//updating the positions of the decorations
	for (GameObject* dec : decor) {
		dec->Update();
	}
	//update positions here	
	player->Update(); //updating the player values	

	for (GameObject* magicBox : magicBoxes) {
		magicBox->Update();
	}
	
	for (GameObject* brickedWall : brickedWalls) {
		brickedWall->Update();
	}
	
	for(GameObject* tile : ground)
	{
		tile->Update();
	}
	
	if (colliders.size() != 0 && !playerHasJumped) {
		grounded = player->Translate(0, (jumpSpeed), colliders);
	}

	if(playerHasJumped){
		bool jumpFlag = player->Translate(0, (-1)*jumpSpeed, colliders);//constantly going upwards
		jumpDistanceTraveled += jumpSpeed; //calculate the distance we have jumped
		jumpSpeed -= jumpVelocity;//increasing our jump speed
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
	SDL_SetRenderDrawColor(this->renderer, 92.0, 148.0, 252.0, 0);
	
	//rendering the decorations
	for (GameObject* dec : decor) {
		dec->Render();
	}

	//Rendering happens here
	player->Render();//rendering the player	

	for (GameObject* magicBox : magicBoxes) {
		magicBox->Render();
	}

	for (GameObject* brickedWalls : brickedWalls) {
		brickedWalls->Render();
	}

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