#include "Game.h"
#include "TextureManager.h"
#include "GameObject.h"
#include <list>

const int WIN_WIDTH=640, WIN_HEIGHT=480;
const int LEVEL_WIDTH = 640 * 4, LEVEL_HEIGHT = 480;

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
std::list<GameObject*> brickedBlocks;
std::list<GameObject*> decor;
std::list<GameObject*> pipes;

//all the colliders in the level
std::list<SDL_Rect> colliders;

//movement animation
std::list<SDL_Texture*> playerMoveAnimation;
SDL_Texture* marioIdleSprite;
SDL_Texture* marioJumpSprite;
SDL_Texture* marioDeadSprite;

//magic box animation
std::list<SDL_Texture*> magicBoxIdleAnimation;
SDL_Texture* magicBoxUsedTexture;
int magicBoxAnimationFrameCounter = 0;
int moveAnimationFrameCounter = 0;
std::list<GameObject*> coinGameObjects;//this list is empty

//Coins
GameObject* coinTemplate; //template coin
std::list<SDL_Texture*> coinAnimation;
int coinAnimationCounter = 0;

//Brick block variables
int brickBlockAnimationCounter = 0;

//goombas
GameObject* nextGoombasRemoval;
std::list<GameObject*> goombasGameObjects;
std::list<SDL_Texture*> goombasMoveAnimation;
SDL_Texture* goombasDeadSprite;
int goombasAnimationFrameCounter = 0;
int goombasDeadCounter = 0;

//Gameobject tags
const std::string PLAYER_TAG = "player";
const std::string BREAKABLE_BRICK_TAG = "breakablebrick";
const std::string MAGIC_BOX_TAG = "magicbox";
const std::string FLOOR_TAG = "floor";
const std::string DECOR_TAG = "decor";
const std::string GOOMBAS_TAG = "goombas";
const std::string PIPE_TAG = "pipe";

//frame counter
int frames = 0;
bool gameover = false;
int gameoverAnimationFrameCounter = 0;

//Camera
SDL_Rect camera = { 0 ,0, WIN_WIDTH, WIN_HEIGHT};

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
		int player_xpos = 70; //11
		player = new GameObject(PLAYER_TAG, "assets/images_custom/mario/mario.bmp", this->renderer, 32 * player_xpos, WIN_HEIGHT - (32 * 3), 24, 32);

		//loading the move right animation
		marioIdleSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario.bmp", this->renderer);
		marioJumpSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario_jump.bmp", this->renderer);
		marioDeadSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario_death.bmp", this->renderer);

		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move0.bmp", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move1.bmp", this->renderer));
		playerMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario_move2.bmp", this->renderer));

		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_0.bmp", this->renderer));
		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_1.bmp", this->renderer));
		magicBoxIdleAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/blocks/blockq_2.bmp", this->renderer));
		magicBoxUsedTexture = TextureManager::LoadTexture("assets/images_custom/blocks/blockq_used.bmp", this->renderer);

		//the ground
		//until the first pit
		//56 is the end of the 4th pipe + 10 offset
		for (int i = 0; i < 56+10; i++) {
			GameObject* temp;
			temp = new GameObject(FLOOR_TAG,"assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject(FLOOR_TAG,"assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i*32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

		for (int i = 68; i < 68+15; i++) {
			GameObject* temp;
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

		for (int i = 68 + 15 + 3; i < 68 + 15 + 64 + 1; i++) {
			GameObject* temp;
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

		//----------------Starting Blocks----------------

		//solo magic box
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG,"assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 13, WIN_HEIGHT- 6 * 32, 32, 32));

		//bottom boxes
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG,"assets/images_custom/blocks/brickred.bmp", this->renderer, 32*17, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 18, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 19, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 20, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 21, WIN_HEIGHT - 6 * 32, 32, 32));

		//top box
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (6+4) * 32, 32, 32));

		//----------------------------Second Part(BLOCKS)----------------------------
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 74, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 75, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 76, WIN_HEIGHT - 6 * 32, 32, 32));
		//---------Upper 8 bricked blocks---------
		int bricked_x = 77;		
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));

		bricked_x += 3;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));

		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));

		//----------BRICKED BLOCK STAR---------------
		bricked_x += 6;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		//star brick
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));

		//THE 4 golden magic boxes
		bricked_x += 5;
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));
		bricked_x += 3;
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));
		//upper magic box
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));
		bricked_x += 3;
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));
		
		//portion before the stairs BRICKED BLOCKS
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));

		//------------------------------Pipes--------------------------------
		//1st pipe
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, 32 * 25, WIN_HEIGHT-(32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, 32 * 26, WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, 32 * 25, WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, 32 * 26, WIN_HEIGHT - (32 * 4), 32, 32));
		
		//2nd longer pipe
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25)+(32*10), WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10), WIN_HEIGHT - (32 * 3), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10), WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10), WIN_HEIGHT - (32 * 4), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, (32 * 25) + (32 * 10), WIN_HEIGHT - (32 * 5), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, (32 * 26) + (32 * 10), WIN_HEIGHT - (32 * 5), 32, 32));

		//3rd longest pipe
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 3), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 4), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 5), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 5), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 6), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8), WIN_HEIGHT - (32 * 6), 32, 32));

		//4th longest pipe
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8) + (32*11), WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 3), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 4), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 5), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 5), 32, 32));

		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, (32 * 25) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 6), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, (32 * 26) + (32 * 10) + (32 * 8) + (32 * 11), WIN_HEIGHT - (32 * 6), 32, 32));
		
		//------------------------------COINS--------------------------------
		coinAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/coin/coin_an0.bmp", this->renderer));
		coinAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/coin/coin_an1.bmp", this->renderer));
		coinAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/coin/coin_an2.bmp", this->renderer));
		coinAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/coin/coin_an3.bmp", this->renderer));		

		coinTemplate = new GameObject("coin", "assets/images_custom/coin/coin_an0.bmp", this->renderer, 32*2, WIN_HEIGHT - (32 * 3), 16, 28);		

		//---------------------------Goombas------------------------------
		goombasMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/enemies/goombas_0.bmp", this->renderer));
		goombasMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/enemies/goombas_1.bmp", this->renderer));		
		goombasDeadSprite = TextureManager::LoadTexture("assets/images_custom/enemies/goombas_ded.bmp", this->renderer);

		//--------------------------Goombas spawning---------------------------

		//Goombas cage used for testing
		/*GameObject* goombasCage = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, 32*10, WIN_HEIGHT - (32*3), 32, 32);
		ground.push_back(goombasCage);		
		goombasCage = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, 32 * 21, WIN_HEIGHT - (32*3), 32, 32);
		ground.push_back(goombasCage);*/

		//goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (32 * 7), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (32 * 3), 32, 32));
		//goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 25, WIN_HEIGHT - (32 * 3), 32, 32));

		//--------------Scene decor--------------
		//First bush large
		decor.push_back(new GameObject(DECOR_TAG,"assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 0, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG,"assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 1, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * 2, WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32*1, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32*2, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32*3, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 3, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 4, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * 2, WIN_HEIGHT - (32 * 4), 32, 32));

		//second grass triple
		int decor_bush = 8;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//third bush small	
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * 13, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * 14, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * 14, WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * 15, WIN_HEIGHT - (32 * 3), 32, 32));

		//fourth grass single
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * 20, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * 21, WIN_HEIGHT - (32 * 3), 32, 32));

		//fifth grass double
		decor_bush = 38;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));		
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//sixth bush large
		decor_bush = 38 + 7;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * (1+decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (2+decor_bush), WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (1+decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32 * (2+decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (3+decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (3+decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (4+decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (2+decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));

		//seventh grass triple
		decor_bush = 38+7+8+3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//eight bush small
		decor_bush = 59+2;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (decor_bush+1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (decor_bush+1), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (decor_bush+2), WIN_HEIGHT - (32 * 3), 32, 32));

		//ninth grass single
		decor_bush = 68;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush+1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush+2), WIN_HEIGHT - (32 * 3), 32, 32));

		//------------------Clouds------------------

		//1st Cloud
		int cloud_position_x = 6;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 3), 32, 32));

		//2nd Cloud
		cloud_position_x = 17;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		//3rd Cloud
		cloud_position_x = 17+8;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));		
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));		

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));		

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 3), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 3), (32 * 3), 32, 32));

		//4th Cloud
		cloud_position_x = 17 + 8 + 9;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));	

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 2), 32, 32));

		//5th Cloud
		cloud_position_x = 17+8+9+15+5;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));

		//6th Cloud
		cloud_position_x = 17 + 8 + 9 + 15 + 5 + 11;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));

		
	}
	else {
		this->isRunning = false;
	}
	
}

void retrieveColliders() {
	//Getting all the colliders from the active objects
	colliders = std::list<SDL_Rect>();

	for (GameObject* magicBox : magicBoxes) {
		colliders.push_back(magicBox->getCollisionBox());
	}

	for (GameObject* brickedWall : brickedBlocks) {
		colliders.push_back(brickedWall->getCollisionBox());
	}

	for (GameObject* ground_tile : ground) {
		colliders.push_back(ground_tile->getCollisionBox());
	}

	for (GameObject* pipe : pipes) {
		colliders.push_back(pipe->getCollisionBox());
	}
}

void Game::handleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}		
		else if (event.type == SDL_KEYDOWN) {					

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

	if (gameover) {
		moveLeftFlag = false;
		moveRightFlag = false;
		playerHasJumped = false;
		grounded = true;
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
	else if (gameover) {		
		player->setScaleH(0.9f);
		player->setScaleW(0.9f);
		player->SetCurrentTexture(marioDeadSprite);

		if (gameoverAnimationFrameCounter <= 30) {
			player->Translate(0, -1.0f, std::list<SDL_Rect>());
		}
		else if (gameoverAnimationFrameCounter > 30) {
			player->Translate(0, 2, std::list<SDL_Rect>());
		}

		gameoverAnimationFrameCounter++;
	}
	else {
		player->SetCurrentTexture(marioIdleSprite);
	}

	if (playerHasJumped || !grounded) {
		player->SetCurrentTexture(marioJumpSprite);
	}
	
	for (GameObject* magicBox : magicBoxes) {
		if (magicBox->getNumberOfHits() < 1) {
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
	}

	for (GameObject* coin : coinGameObjects) {		

		int animFrame = 0;
		if (coinAnimationCounter <= 15) {
			animFrame = 0;			
		}
		else if (coinAnimationCounter <= 30) {
			animFrame = 1;
		}
		else if (coinAnimationCounter <= 45) {
			animFrame = 2;
		}
		else if (coinAnimationCounter <= 60) {
			animFrame = 3;
		}

		auto it = coinAnimation.begin();
		std::advance(it, animFrame);
		coin->SetCurrentTexture(*it);
		coin->Translate(0, -1.5f, std::list<SDL_Rect>());		
	}

	if (coinGameObjects.size()>0 && coinAnimationCounter >= 60) {
		auto it = coinGameObjects.begin();
		std::advance(it, 0);
		coinGameObjects.remove(*it);		
	}

	//------------------Gombas animation----------------------
	for (GameObject* goombas : goombasGameObjects) {
		if (goombas->getNumberOfHits() == 0) {
			int animFrame = 0;
			if (goombasAnimationFrameCounter <= 15) {
				animFrame = 0;
			}
			else if (goombasAnimationFrameCounter <= 30) {
				animFrame = 1;
			}
			else {
				goombasAnimationFrameCounter = 0;
			}

			auto it = goombasMoveAnimation.begin();
			std::advance(it, animFrame);
			goombas->SetCurrentTexture(*it);
		}
		else if (goombas->getNumberOfHits()==1) {
			goombas->SetCurrentTexture(goombasDeadSprite);				
			if (nextGoombasRemoval==NULL) {
				goombasDeadCounter = 0;
				nextGoombasRemoval = goombas;//prepare for removal
				goombas->incrementHit();
			}
		}
	}

	if (goombasDeadCounter >= 30) {
		goombasGameObjects.remove(nextGoombasRemoval);
		nextGoombasRemoval = NULL;
	}
	
	goombasDeadCounter++;
	goombasAnimationFrameCounter++;
	magicBoxAnimationFrameCounter++;
	moveAnimationFrameCounter++;
	coinAnimationCounter++;
	frames++;
}

void Game::update()
{
	float cameraOffsetX = -camera.x;
	
	//update positions here	
	player->setCameraOffset(cameraOffsetX);
	player->Update(); //updating the player values	

	//------------------Updating positions based on the camera offset-------------------
	//updating the positions of the decorations
	for (GameObject* dec : decor) {
		dec->setCameraOffset(cameraOffsetX);
		dec->Update();
	}

	for (GameObject* magicBox : magicBoxes) {
		magicBox->setCameraOffset(cameraOffsetX);
		magicBox->Update();
	}
	
	for (GameObject* brickedWall : brickedBlocks) {
		brickedWall->setCameraOffset(cameraOffsetX);
		brickedWall->Update();
	}	
	
	for(GameObject* tile : ground)
	{
		tile->setCameraOffset(cameraOffsetX);
		tile->Update();
	}

	for (GameObject* coin : coinGameObjects) {
		coin->setCameraOffset(cameraOffsetX);
		coin->Update();
	}

	for (GameObject* goombas : goombasGameObjects) {
		goombas->setCameraOffset(cameraOffsetX);
		goombas->Update();
	}

	for (GameObject* pipe : pipes) {
		pipe->setCameraOffset(cameraOffsetX);
		pipe->Update();
	}

	//retrieving colliders after all the position's are correct
	retrieveColliders();    
	
	//-----------------------Emulating movement so we can detect collisions with certain objects in a certain way---------------------------
	if (playerHasJumped) {
		SDL_Rect predictedRect = player->getCollisionBox();
		predictedRect.y = player->getY() - jumpSpeed;
		for (GameObject* magicBox : magicBoxes) {
			if (magicBox->checkCollision(predictedRect)) {
				magicBox->incrementHit();				
			}
		}

		for (GameObject* brickBlock : brickedBlocks) {
			if (brickBlock->checkCollision(predictedRect)) {
				brickBlock->incrementHit();
				brickBlockAnimationCounter = 0;
			}
		}
	}else if (!grounded) {
		//logic for killing enemies
		
		// KILLING GOOMBAS
		SDL_Rect predictedRect = player->getCollisionBox();
		predictedRect.y = player->getY() + jumpSpeed;
		
		for (GameObject* goombas : goombasGameObjects) {
			if (goombas->getNumberOfHits()==0 && goombas->checkCollision(predictedRect)) {
				goombas->incrementHit();
				goombasDeadCounter = 0;
			}
		}
	}

	for (GameObject* goombas : goombasGameObjects) {
		SDL_Rect predictedGoombasMov = goombas->getCollisionBox();
		predictedGoombasMov.x += (0.5f * goombas->getDirectionX());
		if (player->checkCollision(predictedGoombasMov) && goombas->getNumberOfHits()==0) {
			gameover = true;			
		}
	}

	//----------------------Generic Magic boxes---------------------
	for (GameObject* magicBox : magicBoxes) {
		if (magicBox->getNumberOfHits() == 1) {
			//change the texture to used
			magicBox->SetCurrentTexture(magicBoxUsedTexture);
			//play the coin animation
			GameObject* tempCoin = coinTemplate->clone();
			tempCoin->setX(magicBox->getX()+(magicBox->getCollisionBox().w*0.25f)-(tempCoin->getCollisionBox().w/2));
			tempCoin->setY(magicBox->getY()-magicBox->getCollisionBox().h/2);
			coinGameObjects.push_back(tempCoin);
			coinAnimationCounter = 0;
			magicBox->incrementHit();
		}
	}

	//---------------------Brick blocks logic and animation----------------------
	for (GameObject* brickBlock : brickedBlocks) {
		if (brickBlock->getNumberOfHits() == 1) {
			/*std::cout << "You hit a brick wall" << std::endl;*/

			if (brickBlockAnimationCounter < 10) {
				brickBlock->Translate(0, -0.5f, std::list<SDL_Rect>());
			}
			else if (brickBlockAnimationCounter < 15) {
				brickBlock->Translate(0, 1.0f, std::list<SDL_Rect>());
			}
			else {
				brickBlockAnimationCounter = 0;
				brickBlock->resetHits();
			}								
		}
	}

	//----------------------------Restrict the player inside the camera----------------------------
	if (player->getTag() == PLAYER_TAG && player->getX() <= camera.x) {		
		moveLeftFlag = false;		
	}

	//-----------------------MOVEMENT CODE HERE---------------------------
	if (colliders.size() != 0 && !playerHasJumped && !gameover) {
		grounded = player->Translate(0, (jumpSpeed), colliders);
	}

	//player jump
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

	//moving left
	if (moveLeftFlag) {
		player->Translate(-1 * playerSpeed, 0, colliders);
		playerSpeed += playerVelocity;
	}

	//moving right
	if (moveRightFlag) {
		player->Translate(1 * playerSpeed, 0, colliders);
		playerSpeed += playerVelocity;
	}

	//--------------------Goombas movement--------------------
	for (GameObject* goombas : goombasGameObjects) {	
		if (goombas->getNumberOfHits() == 0) {
			goombas->Translate(0, (jumpSpeed * 0.65), colliders);//allow gravity to pull the goombas down

			bool hit = goombas->TranslateX(0.5f * goombas->getDirectionX(), colliders);
			if (hit) {
				goombas->setDirectionX((-1) * goombas->getDirectionX());
				if (goombas->getDirectionX() < 0) {
					goombas->setFlipX(true);
				}
				else {
					goombas->setFlipX(false);
				}
			}
		}
	}

	brickBlockAnimationCounter++;
	this->Animate();
	this->setCamera();
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

	for (GameObject* coin : coinGameObjects) {
		coin->Render();
	}

	for (GameObject* magicBox : magicBoxes) {
		magicBox->Render();
	}	

	for (GameObject* brickedBlock : brickedBlocks) {
		brickedBlock->Render();
	}

	for (GameObject* tile : ground)
	{
		tile->Render();
	}

	for (GameObject* goombas : goombasGameObjects) {
		goombas->Render();
	}

	for (GameObject* pipe : pipes) {
		pipe->Render();
	}

	player->Render();//rendering the player	

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

void Game::setCamera()
{
	SDL_Rect playerRect = player->getCollisionBox();
	//center the camera over mario 
	//camera.x = (playerRect.x + playerRect.w / 2) - WIN_WIDTH / 2;

	//camera doesn't go backwards only forwards
	if (camera.x < player->getX() - (WIN_WIDTH / 2)) {
		camera.x = player->getX() - (WIN_WIDTH / 2);
	}
	
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	/*
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}*/
}