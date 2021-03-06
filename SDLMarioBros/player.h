#pragma once

#include "graphics.h"
#include "texture.h"
#include "timer.h"
#include "levelcontent.h"
#include "tile.h"
#include "util.h"
#include "sound.h"
#include "animatedsprite.h"


const int PLAYER_SPEED = 1;
const int JUMP_STRENGTH = -2;

enum Player_PowerUpStates
{
	PLAYER_SMALL,
	PLAYER_GRAND,
	PLAYER_FIRE,
	PLAYER_STAR
};

class Player
{
public:
	// Basic functions (Construct/Destructors + Draw/Update)
	Player();
	Player(Graphics* graph,int x, int y);
	~Player();
	void Update(LevelContent &content, Graphics* graphics, Sound* sound);
	void Draw(Graphics* graph, int camX, int camY);
	

	// Player's actions
	void Idle();
	void MoveLeft();
	void MoveRight();
	void Jump(Sound* sound);
	void Sprint();
	void Unsprint();
	void Fire(Graphics* graph, std::vector<Bullet>* bullets);
	void Hit();
	void Reset();

	// Player's States
	bool isOnGround();
	bool isJumping();
	bool isDead();

	// Accessors
	int GetX();
	int GetY();
	int GetWidth();
	int GetHeight();
	int GetScore();
	int GetCoins();
	SDL_Rect GetRect();
	SDL_Rect GetOffset(int pwrup);

	// Setters
	void SetX(int x);
	void SetY(int y);

private:
	void setupAnimations();
	
	int m_x;
	int m_y;
	int m_velx;
	int m_vely;
	bool m_jumping;
	bool m_dead;
	bool m_onGround;
	bool m_direction;
	bool m_sprint;
	int m_pwrupState;
	AnimatedSprite* m_sprite;
	Timer m_timer;
	int m_score = 0;
	int m_coins = 0;
};