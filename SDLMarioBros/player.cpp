#include "player.h"

Player::Player()
{
	this->m_x = 0;
	this->m_y = 0;
	this->m_sprite = NULL;
	this->m_velx = 0;
	this->m_vely = 0;
}

Player::Player(Graphics* graph,int x, int y)
{
	Player();
	this->m_x = x;
	this->m_y = y;
	this->m_sprite = new Texture(graph, "sprites/mario.png");
}

Player::~Player()
{
	//this->m_sprite->Free();
}

void Player::Update()
{
	this->m_x += m_velx;
	this->m_y += m_vely;
	SDL_Delay(4);			// Need to find another way to slow instead of delay
}

void Player::Draw(Graphics* graph, int camX, int camY)
{
	if (m_sprite != NULL) {
		SDL_Rect destRect = { this->m_x - camX, this->m_y - camY,this->m_sprite->GetWidth(), this->m_sprite->GetHeight() };
		this->m_sprite->Draw(graph, &destRect);
	}
}

void Player::Idle()
{
	this->m_velx = 0;
}

void Player::MoveLeft()
{
	this->m_velx = -PLAYER_SPEED;
}

void Player::MoveRight()
{
	this->m_velx = PLAYER_SPEED;
}

int Player::GetX()
{
	return this->m_x;
}

int Player::GetY()
{
	return this->m_y;
}

int Player::GetWidth()
{
	return this->m_sprite->GetWidth();
}

int Player::GetHeight()
{
	return this->m_sprite->GetHeight();
}
