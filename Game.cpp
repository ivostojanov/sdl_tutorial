#include "Game.h"
#include "TextureManager.h"
#include "GameObject.h"
#include <list>

const int WIN_WIDTH=640, WIN_HEIGHT=480;
const int LEVEL_WIDTH = 640 * 4, LEVEL_HEIGHT = 480;

//Main player variables
GameObject* player;
float playerSpeed=2.5f;
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

//Invisible box
GameObject* invisibleBox;
bool invisibleBoxShow=false;


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
GameObject* brickedBlockWithCoins;
int brickedBlockCoinCounter = 0;
int brickedBlockWithCoinsAnimationCounter = 0;

std::list<GameObject*> goombasGameObjects;
std::list<SDL_Texture*> goombasMoveAnimation;
SDL_Texture* goombasDeadSprite;
int goombasAnimationFrameCounter = 0;
int goombasDeadCounter = 0;

//Gameobject tags
const std::string PLAYER_TAG = "player";
const std::string BREAKABLE_BRICK_TAG = "breakablebrick";
const std::string MAGIC_BOX_TAG = "magicbox";
const std::string MAGIC_BOX_WITH_SHROOM = "magicbox_with_shroom";
const std::string FLOOR_TAG = "floor";
const std::string DECOR_TAG = "decor";
const std::string GOOMBAS_TAG = "goombas";
const std::string PIPE_TAG = "pipe";
const std::string LEVEL_UP_MUSHROOM = "level_up_mushrom";
const std::string POWER_UP_MUSHROOM = "power_up_mushroom";

//frame counter
int frames = 0;
bool gameover = false;
int gameoverAnimationFrameCounter = 0;

//big mario
int isMarioBig = 0;
int marioGettingBigAnimationCounter = 0;
bool squat = false;
SDL_Texture* bigMarioIdleSprite;
std::list<SDL_Texture*> bigMarioMoveAnimation;
SDL_Texture* bigMarioJumpSprite;
SDL_Texture* bigMarioSquatSprite;
int marioGetsSmallerCounter = 0;

//breakable block fragments objects
GameObject* brickedBlockFragmentTemplate;
std::list<GameObject*> brickedBlockFragments;

//power up mushroom
GameObject* powerUpMushroomTemplate;
std::list<GameObject*> mushrooms;

//level mushroom
GameObject* levelUpMushroomTemplate;

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
		int player_xpos = 7; //11 70 170
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

		for (int i = 68 + 15 + 3; i < 68 + 15 + 64 + 1+2; i++) {
			GameObject* temp;
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 32, 32, 32);
			ground.push_back(temp);
			temp = new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red_1.bmp", this->renderer, (i * 32), WIN_HEIGHT - 64, 32, 32);
			ground.push_back(temp);
		}

		for (int i = 150+2; i < 300; i++) {
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
		
		magicBoxes.push_back(new GameObject(MAGIC_BOX_WITH_SHROOM, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 18, WIN_HEIGHT - 6 * 32, 32, 32));
		
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 19, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 20, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 21, WIN_HEIGHT - 6 * 32, 32, 32));

		//top box
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 19, WIN_HEIGHT - (6+4) * 32, 32, 32));

		//----------------------------Second Part(BLOCKS)----------------------------
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * 74, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_WITH_SHROOM, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * 75, WIN_HEIGHT - 6 * 32, 32, 32));
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

		//coin brick box
		brickedBlockWithCoins = new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32);
		//brickedBlocks.push_back(brickedBlockWithCoins);

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
		bricked_x += 6;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));

		bricked_x += 3;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));

		bricked_x += 5;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 10 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 10 * 32, 32, 32));

		//stairs
		bricked_x+=3;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32*3, 32, 32));
		
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32*3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32*4, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 5, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		//stairs inverse
		bricked_x += 3;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 5, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 4, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));

		//larger stairs
		bricked_x += 5;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 3, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 4, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 5, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		//stairs inverse
		bricked_x += 3;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 5, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 4, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));

		//small pipe
		bricked_x += 5;
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 3), 32, 32));		
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 4), 32, 32));

		//3 bricked and one magic box
		bricked_x += 4;
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		magicBoxes.push_back(new GameObject(MAGIC_BOX_TAG, "assets/images_custom/blocks/blockq_0.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - 6 * 32, 32, 32));
		brickedBlocks.push_back(new GameObject(BREAKABLE_BRICK_TAG, "assets/images_custom/blocks/brickred.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - 6 * 32, 32, 32));

		//small pipe
		bricked_x += 8;
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_bot.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_left_top.bmp", this->renderer, 32 * bricked_x++, WIN_HEIGHT - (32 * 4), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_bot.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 3), 32, 32));
		pipes.push_back(new GameObject(PIPE_TAG, "assets/images_custom/pipe/pipe_right_top.bmp", this->renderer, 32 * bricked_x, WIN_HEIGHT - (32 * 4), 32, 32));

		//LARGEST STAIRS
		bricked_x += 1;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 3, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 4, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 5, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 6, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 7, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 7, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 8, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 7, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 8, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 9, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 7, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 8, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 9, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x++ * 32), WIN_HEIGHT - 32 * 10, 32, 32));

		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 4, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 5, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 6, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 7, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 8, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 9, 32, 32));
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 10, 32, 32));		


		//------------------------------Pipes--------------------------------
		//1st pipe small
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

		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 38, WIN_HEIGHT - (32 * 3), 32, 32));

		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 48, WIN_HEIGHT - (32 * 3), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 50, WIN_HEIGHT - (32 * 3), 32, 32));

		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 90, WIN_HEIGHT - (32 * 40), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 92, WIN_HEIGHT - (32 * 50), 32, 32));

		//2 starter goombas


		//tree boxes goombas
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 131, WIN_HEIGHT - (32 * 50), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 129, WIN_HEIGHT - (32 * 50), 32, 32));

		//the 4 goombas
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 146, WIN_HEIGHT - (32 * 70), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 144, WIN_HEIGHT - (32 * 70), 32, 32));

		//last goombas
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 175, WIN_HEIGHT - (32 * 3), 32, 32));
		goombasGameObjects.push_back(new GameObject(GOOMBAS_TAG, "assets/images_custom/enemies/goombas_0.bmp", this->renderer, 32 * 173, WIN_HEIGHT - (32 * 3), 32, 32));

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

		//10th grass double
		decor_bush += 15+3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//11th bush large
		decor_bush += 3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (4 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));

		//12th grass triple
		decor_bush += 6+4+1;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));

		//13th bush small		
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (decor_bush + 2), WIN_HEIGHT - (32 * 3), 32, 32));

		//14th grass small
		decor_bush += 4+3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush + 2), WIN_HEIGHT - (32 * 3), 32, 32));

		//15th grass double
		decor_bush +=7+11;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush++), WIN_HEIGHT - (32 * 3), 32, 32));		

		//16th bush large
		decor_bush += 3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (4 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));

		//17th singular grass
		decor_bush += 10+3;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush + 2), WIN_HEIGHT - (32 * 3), 32, 32));

		//18th small bush
		decor_bush += 1+2;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (decor_bush + 2), WIN_HEIGHT - (32 * 3), 32, 32));

		//19th singular grass
		decor_bush += 7;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_left.bmp", this->renderer, 32 * (decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_center.bmp", this->renderer, 32 * (decor_bush + 1), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/grass_right.bmp", this->renderer, 32 * (decor_bush + 2), WIN_HEIGHT - (32 * 3), 32, 32));

		//20th large bush
		decor_bush += 20+5;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * decor_bush, WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_left.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_top.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 5), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (1 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_1.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (3 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_right.bmp", this->renderer, 32 * (4 + decor_bush), WIN_HEIGHT - (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/bush_center_0.bmp", this->renderer, 32 * (2 + decor_bush), WIN_HEIGHT - (32 * 4), 32, 32));

		//flag decor
		bricked_x += 1+6+2;
		ground.push_back(new GameObject(FLOOR_TAG, "assets/images_custom/blocks/gnd_red2.bmp", this->renderer, (bricked_x * 32), WIN_HEIGHT - 32 * 3, 32, 32));

		//------------------Clouds------------------

		//1st Cloud singular
		int cloud_position_x = 6;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x+1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x-1), (32 * 3), 32, 32));

		//2nd Cloud singular
		cloud_position_x = 17;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		//3rd Cloud triple
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

		//4th Cloud double
		cloud_position_x = 17 + 8 + 9;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));	

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 2), 32, 32));

		//5th Cloud singular
		cloud_position_x = 17+8+9+15+5;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));

		//6th Cloud singular
		cloud_position_x = 17 + 8 + 9 + 15 + 5 + 11;//65
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));

		//7th cloud triple
		cloud_position_x += 8;
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

		//8th cloud double
		cloud_position_x += 9;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 2), 32, 32));

		//9th singular
		cloud_position_x += 20;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		//10th singular
		cloud_position_x += 10+1;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		//11th triple
		cloud_position_x += 8;
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

		//12th cloud double
		cloud_position_x += 9;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 2), 32, 32));

		//13th cloud singular
		cloud_position_x += 20;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 4), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));

		//14th cloud singular
		cloud_position_x += 10+1;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		//15th cloud triple
		cloud_position_x += 8;
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

		//16th cloud double
		cloud_position_x += 9;
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_bot.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_left_top.bmp", this->renderer, 32 * (cloud_position_x - 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * cloud_position_x, (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * cloud_position_x, (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_bot.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_center_top.bmp", this->renderer, 32 * (cloud_position_x + 1), (32 * 2), 32, 32));

		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_bot.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 3), 32, 32));
		decor.push_back(new GameObject(DECOR_TAG, "assets/images_custom/decor/cloud_right_top.bmp", this->renderer, 32 * (cloud_position_x + 2), (32 * 2), 32, 32));

		//Invisible box init
		invisibleBox = new GameObject("INVISIBLE_BOX", "assets/images_custom/blocks/blockq_used.bmp", this->renderer, 32 * 61, WIN_HEIGHT - (32 * 7), 32, 32);

		//Big Mario textures
		bigMarioIdleSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario1.bmp", this->renderer);
		bigMarioJumpSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario1_jump.bmp", this->renderer);
		bigMarioMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario1_move0.bmp", this->renderer));
		bigMarioMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario1_move1.bmp", this->renderer));
		bigMarioMoveAnimation.push_back(TextureManager::LoadTexture("assets/images_custom/mario/mario1_move2.bmp", this->renderer));
		bigMarioSquatSprite = TextureManager::LoadTexture("assets/images_custom/mario/mario1_squat.bmp", this->renderer);

		//bricked block parts
		brickedBlockFragmentTemplate = new GameObject("BRICKED_FRAGMENT", "assets/images_custom/blocks/bricked_fragment.png", this->renderer, 32*5, 32*5, 16, 16);
		
		//power up mushroom template
		powerUpMushroomTemplate = new GameObject(POWER_UP_MUSHROOM, "assets/images_custom/mushrooms/mushroom.bmp", this->renderer, 32*5, 32*5, 32, 32);
		powerUpMushroomTemplate->setDirectionX(1);		

		//level up mushroom
		levelUpMushroomTemplate = new GameObject(LEVEL_UP_MUSHROOM, "assets/images_custom/mushrooms/mushroom_1up.bmp", this->renderer, 32 * 4, 32 * 5, 32, 32);
		levelUpMushroomTemplate->setDirectionX(1);	

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

	colliders.push_back(brickedBlockWithCoins->getCollisionBox());

	if(invisibleBoxShow)
		colliders.push_back(invisibleBox->getCollisionBox());
}

void Game::handleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
		else if (isMarioBig == 1) {

		}
		else if (event.type == SDL_KEYDOWN && !squat) {					

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
			else if (event.key.keysym.sym == SDLK_s && isMarioBig==2) {
				if(!squat)
					player->setY(player->getY() + 20);
				player->setSpriteHeight(44);
				squat = true;
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
				case SDLK_s:
					if (isMarioBig == 2) {
						if (squat)
							player->setY(player->getY() - 20);
						player->setSpriteHeight(64);
						squat = false;
					}
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

	//big mario logic
	if (isMarioBig == 1) {
		moveLeftFlag = false;
		moveRightFlag = false;	
		playerHasJumped = false;
		if (marioGettingBigAnimationCounter < 10) {
			player->setSpriteWidth(32);
			player->setSpriteHeight(64);
			if(marioGettingBigAnimationCounter==0)
				player->setY(player->getY() - 32);
			player->SetCurrentTexture(bigMarioIdleSprite);						
			marioGettingBigAnimationCounter++;
		}
		else if (marioGettingBigAnimationCounter < 20) {
			player->setSpriteWidth(24);
			player->setSpriteHeight(32);
			if (marioGettingBigAnimationCounter == 10)
				player->setY(player->getY() + 32);
			player->SetCurrentTexture(marioIdleSprite);
			marioGettingBigAnimationCounter++;
		}
		else if (marioGettingBigAnimationCounter < 30) {
			player->setSpriteWidth(32);
			player->setSpriteHeight(64);
			if (marioGettingBigAnimationCounter == 20)
				player->setY(player->getY() - 32);
			player->SetCurrentTexture(bigMarioIdleSprite);
			marioGettingBigAnimationCounter++;
		}
		else if (marioGettingBigAnimationCounter < 40) {
			player->setSpriteWidth(24);
			player->setSpriteHeight(32);
			if (marioGettingBigAnimationCounter == 30)
				player->setY(player->getY() + 32);			
			player->SetCurrentTexture(marioIdleSprite);						
			marioGettingBigAnimationCounter++;
		}		
		else {
			player->setSpriteWidth(32);
			player->setSpriteHeight(64);
			player->setY(player->getY() - 32);			
			isMarioBig++;
		}
	}
	else if (isMarioBig <= 0) {
		player->setSpriteWidth(24);
		player->setSpriteHeight(32);
	}

	
	if (moveRightFlag) {
		player->setFlipX(false);		
		
		int animFrame = 0;
		if (moveAnimationFrameCounter <= 5) {
			animFrame = 2;
		}
		else if (moveAnimationFrameCounter <= 10) {
			animFrame = 1;
		}
		else if (moveAnimationFrameCounter <= 15) {
			animFrame = 0;
		}
		else {
			moveAnimationFrameCounter = 0;
		}	

		auto it = isMarioBig <= 0?playerMoveAnimation.begin():bigMarioMoveAnimation.begin();
		std::advance(it, animFrame);		
		
		player->SetCurrentTexture(*it);
	}
	else if (moveLeftFlag) {
		player->setFlipX(true);
		int animFrame = 0;

		if (moveAnimationFrameCounter <= 5) {
			animFrame = 0;
		}
		else if (moveAnimationFrameCounter <= 10) {
			animFrame = 1;
		}
		else if (moveAnimationFrameCounter <= 15) {
			animFrame = 2;
		}
		else {
			moveAnimationFrameCounter = 0;
		}

		if (isMarioBig <= 0 && !squat) {
			for (SDL_Texture* texture : playerMoveAnimation) {
				if (animFrame == 0) {
					player->SetCurrentTexture(texture);
					break;
				}
				animFrame--;
			}
		}
		else if(isMarioBig==2 && !squat) {
			for (SDL_Texture* texture : bigMarioMoveAnimation) {
				if (animFrame == 0) {
					player->SetCurrentTexture(texture);
					break;
				}
				animFrame--;
			}
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
		if (squat) {						
			player->SetCurrentTexture(bigMarioSquatSprite);
		}
		else {
			if (isMarioBig <= 0)
				player->SetCurrentTexture(marioIdleSprite);
			else if(isMarioBig>1)
				player->SetCurrentTexture(bigMarioIdleSprite);
		}
	}

	if (playerHasJumped || !grounded) {
		if (isMarioBig <= 0 && !squat) {//meaning mario is small
			player->SetCurrentTexture(marioJumpSprite);
		}
		else if(isMarioBig == 2 && !squat) {
			player->SetCurrentTexture(bigMarioJumpSprite);
		}		
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
		if (coin->getAnimationCounter() <= 15) {
			animFrame = 0;			
		}
		else if (coin->getAnimationCounter() <= 8) {
			animFrame = 1;
		}
		else if (coin->getAnimationCounter() <= 16) {
			animFrame = 2;
		}
		else if (coin->getAnimationCounter() <= 24) {
			animFrame = 3;
		}
		else if (coin->getAnimationCounter() <= 32) {
			animFrame = 2;
		}
		else if (coin->getAnimationCounter() <= 40) {
			animFrame = 1;
		}
		else if (coin->getAnimationCounter() <= 48) {
			animFrame = 0;
		}

		auto it = coinAnimation.begin();
		std::advance(it, animFrame);
		coin->SetCurrentTexture(*it);

		if (coin->getAnimationCounter() <= 32)
			coin->Translate(0, -2.5f, std::list<SDL_Rect>());
		else
			coin->Translate(0, 4.0f, std::list<SDL_Rect>());
		coin->addToAnimationCounter(1);
	}

	if (coinGameObjects.size()>0 && coinGameObjects.front()->getAnimationCounter() > 48) {
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
		else if (goombas->getNumberOfHits() == 1 && !goombas->getFlipY()) {
			goombas->SetCurrentTexture(goombasDeadSprite);				
			if (nextGoombasRemoval==NULL) {
				goombasDeadCounter = 0;
				nextGoombasRemoval = goombas;//prepare for removal
				goombas->incrementHit();
			}
		}//turn them over and drop them out of the stage
		else if (goombas->getNumberOfHits() >= 1 && goombas->getFlipY()) {
			goombas->setFlipY(true);
			goombas->Translate(0, (jumpSpeed * 1.0), std::list<SDL_Rect>());//falling out of the stage			
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

	brickedBlockWithCoins->setCameraOffset(cameraOffsetX);
	brickedBlockWithCoins->Update();

	invisibleBox->setCameraOffset(cameraOffsetX);
	invisibleBox->Update();
	
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

	for (GameObject* powerUpShroom : mushrooms) {
		powerUpShroom->setCameraOffset(cameraOffsetX);
		powerUpShroom->Update();
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
		bool hit = false;

		for (GameObject* magicBox : magicBoxes) {
			if (!hit && magicBox->checkCollision(predictedRect) && magicBox->getNumberOfHits()==0) {
				magicBox->incrementHit();				
				hit=true;//maybe change this hit
				break;
			}
		}

		for (GameObject* brickBlock : brickedBlocks) {
			if (!hit && brickBlock->checkCollision(predictedRect)) {
				brickBlock->incrementHit();
				brickBlockAnimationCounter = 0;
				hit = true;				
				break;
			}
		}
				
		if (!hit && brickedBlockWithCoins->checkCollision(predictedRect)) {
			brickedBlockWithCoins->incrementHit();
			brickedBlockWithCoinsAnimationCounter = 0;
			hit = true;
		}

		//show the invisible box once the player has hit it from below
		if (!hit && !invisibleBoxShow && invisibleBox->checkCollision(predictedRect) && moveLeftFlag==false && moveRightFlag==false) {
			invisibleBoxShow = true;			
			hit = true;
			//spawn level up mushroom here			
			GameObject* tempMushroom = levelUpMushroomTemplate->clone();
			tempMushroom->setX(invisibleBox->getX());
			tempMushroom->setY(invisibleBox->getY() - invisibleBox->getCollisionBox().h / 2);
			mushrooms.push_back(tempMushroom);
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

		//Killing goombas from a block hit		
		for (GameObject* brickBlock : brickedBlocks) {
			for (GameObject* goombas : goombasGameObjects) {
				if (goombas->checkCollision(brickBlock->getCollisionBox()) && brickBlock->getNumberOfHits() == 1) {
					goombas->incrementHit();
					goombas->setFlipY(true);
					goombasDeadCounter = 0;
				}
			}
		}
		
		////this code is not working correctly
		//for (GameObject* magicBox : magicBoxes) {
		//	for (GameObject* goombas : goombasGameObjects) {
		//		if (goombas->checkCollision(magicBox->getCollisionBox()) && magicBox->getNumberOfHits() >= 1) {
		//			goombas->incrementHit();
		//			goombasDeadCounter = 0;
		//		}
		//	}
		//}
		

	}

	//Goombas collision with player
	for (GameObject* goombas : goombasGameObjects) {
		SDL_Rect predictedGoombasMov = goombas->getCollisionBox();
		predictedGoombasMov.x += (0.5f * goombas->getDirectionX());
		if (player->checkCollision(predictedGoombasMov) && goombas->getNumberOfHits()==0 && isMarioBig==0) {
			gameover = true;			
		}
		else if (player->checkCollision(predictedGoombasMov) && goombas->getNumberOfHits() == 0 && isMarioBig > 1) {
			isMarioBig = -1;
			marioGetsSmallerCounter = 0;
			player->setY(player->getY() + 32);
		}
	}

	//Power up collision with player
	bool destroyMushroom = false;
	for (GameObject* powerUpShroom : mushrooms) {
		SDL_Rect predictedShroomMov = powerUpShroom->getCollisionBox();
		predictedShroomMov.x += (0.5f * powerUpShroom->getDirectionX());
		if (player->checkCollision(predictedShroomMov) && powerUpShroom->getTag() == POWER_UP_MUSHROOM) {
			isMarioBig = isMarioBig == 0 ? 1 : isMarioBig;
			marioGettingBigAnimationCounter = 0;
			destroyMushroom = true;
		}
		else if (player->checkCollision(predictedShroomMov) && powerUpShroom->getTag() == LEVEL_UP_MUSHROOM) {
			destroyMushroom = true;
		}
	}

	if (destroyMushroom) {
		mushrooms.pop_front();
	}

	//----------------------Generic Magic boxes---------------------START THE COIN ANIMATION-----------------------
	for (GameObject* magicBox : magicBoxes) {
		if (magicBox->getNumberOfHits() == 1 && magicBox->getTag()==MAGIC_BOX_TAG) {
			//change the texture to used
			magicBox->SetCurrentTexture(magicBoxUsedTexture);
			//play the coin animation
			GameObject* tempCoin = coinTemplate->clone();
			tempCoin->setX(magicBox->getX()+(magicBox->getCollisionBox().w*0.25f)-(tempCoin->getCollisionBox().w/2));
			tempCoin->setY(magicBox->getY()-magicBox->getCollisionBox().h/2);
			coinGameObjects.push_back(tempCoin);
			coinAnimationCounter = 0;
			magicBox->incrementHit();

			if (coinGameObjects.size() > 1)
				coinAnimationCounter = 61;
		}
		else if (magicBox->getNumberOfHits() == 1 && magicBox->getTag() == MAGIC_BOX_WITH_SHROOM) {
			//change the texture to used
			magicBox->SetCurrentTexture(magicBoxUsedTexture);

			//spawn the mushroom
			GameObject* tempMushroom = powerUpMushroomTemplate->clone();
			tempMushroom->setX(magicBox->getX());// + (magicBox->getCollisionBox().w * 0.25f) - (tempMushroom->getCollisionBox().w / 2)
			tempMushroom->setY(magicBox->getY() - magicBox->getCollisionBox().h / 2);
			mushrooms.push_back(tempMushroom);

			magicBox->incrementHit();
		}
	}

	//---------------------Brick blocks logic and animation----------------------	
	int indexToDelete = 0;
	for (GameObject* brickBlock : brickedBlocks) {
		if (brickBlock->getNumberOfHits() == 1 && isMarioBig==0) {
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
		else if (brickBlock->getNumberOfHits() == 1 && isMarioBig > 0) {
			//spawn the animation of small blocks
			GameObject* tempFragments = brickedBlockFragmentTemplate->clone();
			tempFragments->setDirectionX(-1);
			tempFragments->setX(brickBlock->getX() + (brickBlock->getCollisionBox().w * 0.25f) - (tempFragments->getCollisionBox().w / 2));
			tempFragments->setY(brickBlock->getY() - brickBlock->getCollisionBox().h / 2);
			brickedBlockFragments.push_back(tempFragments);

			tempFragments = brickedBlockFragmentTemplate->clone();
			tempFragments->setDirectionX(-2);
			tempFragments->setX(brickBlock->getX() + (brickBlock->getCollisionBox().w * 0.25f) - (tempFragments->getCollisionBox().w / 2));
			tempFragments->setY(brickBlock->getY() - brickBlock->getCollisionBox().h * 1);
			brickedBlockFragments.push_back(tempFragments);

			tempFragments = brickedBlockFragmentTemplate->clone();
			tempFragments->setDirectionX(1);
			tempFragments->setX(brickBlock->getX() + (brickBlock->getCollisionBox().w * 0.25f) - (tempFragments->getCollisionBox().w / 2));
			tempFragments->setY(brickBlock->getY() - brickBlock->getCollisionBox().h / 2);
			brickedBlockFragments.push_back(tempFragments);

			tempFragments = brickedBlockFragmentTemplate->clone();
			tempFragments->setDirectionX(2);
			tempFragments->setX(brickBlock->getX() + (brickBlock->getCollisionBox().w * 0.25f) - (tempFragments->getCollisionBox().w / 2));
			tempFragments->setY(brickBlock->getY() - brickBlock->getCollisionBox().h * 1);
			brickedBlockFragments.push_back(tempFragments);

			// doesn't seem to work
			////kill goombas in contact with
			//for (GameObject* goombas : goombasGameObjects) {
			//	SDL_Rect box_rect = brickBlock->getCollisionBox();
			//	box_rect.h = 64;
			//	box_rect.y += 32;
			//	if (goombas->checkCollision(box_rect)) {
			//		goombas->incrementHit();
			//		goombas->setFlipY(true);
			//		goombasDeadCounter = 0;
			//	}
			//}

			break;
		}
		indexToDelete++;
	}

	//destroying blocks	
	if (indexToDelete < brickedBlocks.size()) {
		auto it = brickedBlocks.begin();
		std::advance(it, indexToDelete);
		brickedBlocks.remove(*it);
	}
	
	//----------------------Bricked Box wtih coins---------------------
	if (brickedBlockWithCoins->getNumberOfHits() == 1) {
		//play bricked animation
		if (brickedBlockWithCoinsAnimationCounter == 0) {
			brickedBlockWithCoins->Translate(0, -0.5f, std::list<SDL_Rect>());
			//play the coin animation
			GameObject* tempCoin = coinTemplate->clone();
			tempCoin->setX(brickedBlockWithCoins->getX() + (brickedBlockWithCoins->getCollisionBox().w * 0.25f) - (tempCoin->getCollisionBox().w / 2));
			tempCoin->setY(brickedBlockWithCoins->getY() - brickedBlockWithCoins->getCollisionBox().h / 2);
			coinGameObjects.push_back(tempCoin);
			coinAnimationCounter = 0;
		}
		
		if (brickedBlockWithCoinsAnimationCounter < 10) {
			brickedBlockWithCoins->Translate(0, -0.5f, std::list<SDL_Rect>());
		}
		else if (brickedBlockWithCoinsAnimationCounter < 15) {
			brickedBlockWithCoins->Translate(0, 1.0f, std::list<SDL_Rect>());
		}
		else {
			brickedBlockWithCoinsAnimationCounter = 0;
			brickedBlockCoinCounter++;
			brickedBlockWithCoins->resetHits();
		}

		if (brickedBlockCoinCounter == 5) {
			//change the texture to used
			brickedBlockWithCoins->SetCurrentTexture(magicBoxUsedTexture);
			brickedBlockWithCoins->incrementHit();
			brickedBlockWithCoins->incrementHit();
		}

		brickedBlockWithCoinsAnimationCounter++;
	}

	//----------------------------Restrict the player inside the camera----------------------------
	if (player->getTag() == PLAYER_TAG && player->getX() <= camera.x) {		
		moveLeftFlag = false;		
	}

	//-----------------------MOVEMENT CODE HERE---------------------------
	if (colliders.size() != 0 && !playerHasJumped && !gameover && isMarioBig!=1) {
		grounded = player->Translate(0, (jumpSpeed), colliders);
	}

	//player jump
	if(playerHasJumped && isMarioBig!=1){
		bool jumpFlag = player->Translate(0, (-1)*jumpSpeed, colliders);//constantly going upwards (Gravity)
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

	//---------------Goombas custom collider for collision with each other---------------------
	/*std::list<SDL_Rect> custom_colliders;
	for (GameObject* goombas : goombasGameObjects) {
		custom_colliders.push_back(goombas->getCollisionBox());
	}*/

	//--------------------Goombas movement--------------------
	for (GameObject* goombas : goombasGameObjects) {	
		if (goombas->getNumberOfHits() == 0) {
			goombas->Translate(0, (jumpSpeed * 0.65), colliders);//allow gravity to pull the goombas down

			bool hit = goombas->TranslateX(0.5f * goombas->getDirectionX(), colliders);			
			//hit = hit == false ? goombas->TranslateX(0.5f * goombas->getDirectionX(), custom_colliders) : false;
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

	//--------------------Mushroom movement--------------------
	for (GameObject* powerUpShroom : mushrooms) {

		if (powerUpShroom->getAnimationCounter() < 60) {
			powerUpShroom->Translate(0, (jumpSpeed * -0.075), std::list<SDL_Rect>());
			powerUpShroom->addToAnimationCounter(1);
		}
		else {
			powerUpShroom->Translate(0, (jumpSpeed * 0.65), colliders);//allow gravity to pull the mushrooms down

			bool hit = powerUpShroom->TranslateX(1.5f * powerUpShroom->getDirectionX(), colliders);
			//hit = hit == false ? goombas->TranslateX(0.5f * goombas->getDirectionX(), custom_colliders) : false;
			if (hit) {
				powerUpShroom->setDirectionX((-1) * powerUpShroom->getDirectionX());

				//jump the shroom
				powerUpShroom->Translate(12.0f * powerUpShroom->getDirectionX(), -12.0f, std::list<SDL_Rect>());

				if (powerUpShroom->getDirectionX() < 0) {
					powerUpShroom->setFlipX(true);
				}
				else {
					powerUpShroom->setFlipX(false);
				}
			}
		}				
	}

	//temporary BRICKED BLOCK FRAGMENT
	for (GameObject* brickedFragments : brickedBlockFragments) {
		brickedFragments->setCameraOffset(cameraOffsetX);		

		if (brickedFragments->getAnimationCounter() < 15)
			brickedFragments->Translate(0.5 * brickedFragments->getDirectionX(), -3.0, std::list<SDL_Rect>());
		else if (brickedFragments->getAnimationCounter() >= 15) {
			brickedFragments->Translate(0.5 * brickedFragments->getDirectionX(), brickedFragments->getAnimationCounter()/8, std::list<SDL_Rect>());
		}

		brickedFragments->addToAnimationCounter(1);
		brickedFragments->Update();
	}

	if (brickedBlockFragments.size() > 0 && brickedBlockFragments.front()->getAnimationCounter()>60*10) {
		auto it = brickedBlockFragments.begin();
		std::advance(it, 0);		
		brickedBlockFragments.remove(*it);
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

	for (GameObject* powerUpShroom : mushrooms) {
		powerUpShroom->Render();
	}

	for (GameObject* coin : coinGameObjects) {
		coin->Render();
	}

	for (GameObject* magicBox : magicBoxes) {
		magicBox->Render();
	}	

	for (GameObject* brickedBlock : brickedBlocks) {
		brickedBlock->Render();
	}

	brickedBlockWithCoins->Render();
	if(invisibleBoxShow)
		invisibleBox->Render();	

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

	if (isMarioBig == -1) {
		if (marioGetsSmallerCounter <= 5) {
			player->Render();
		}
		else if (marioGetsSmallerCounter <= 10) {
			
		}
		else if (marioGetsSmallerCounter <= 15) {
			player->Render();
		}
		else if (marioGetsSmallerCounter <= 20) {
			
		}
		else if (marioGetsSmallerCounter <= 25) {
			player->Render();
		}
		else if (marioGetsSmallerCounter <= 30) {
			
		}
		else if (marioGetsSmallerCounter <= 60) {
			player->Render();
		}
		else if (marioGetsSmallerCounter > 60) {
			player->Render();
			isMarioBig = 0;
		}
		marioGetsSmallerCounter++;
	}
	else {
		player->Render();//rendering the player	
	}

	for (GameObject* brickedFragments : brickedBlockFragments) {
		brickedFragments->Render();
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