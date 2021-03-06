#include "player.h"


Player::Player()
{
	this->m_x = 0;
	this->m_y = 0;
	this->m_sprite = NULL;
	this->m_velx = 0;
	this->m_vely = 0;
	this->m_onGround = false;
	this->m_jumping = false;
	this->m_direction = false;
	this->m_sprint = false;
	this->m_dead = false;
	this->m_pwrupState = PLAYER_SMALL;
}

Player::Player(Graphics* graph,int x, int y)
{
	Player();
	this->m_x = x;
	this->m_y = y;
	this->m_sprite = new AnimatedSprite(graph, "sprites/mariosheet.png");
	this->setupAnimations();
}

Player::~Player()
{
	//this->m_sprite->Free();
}

void Player::Update(LevelContent &content,Graphics* graph, Sound* sound)
{
	
	
	// Collision detection (to factorize into checkCollision function)
	SDL_Rect groundPlayerHitBox = { this->m_x,this->m_y + 3, this->GetWidth(), this->GetHeight() };
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		if (content.tileMap[i]->CheckCollision(groundPlayerHitBox) && content.tileMap[i]->GetValue() > 0 && !this->m_dead) {
			this->m_onGround = true;
			break;
		}
		else
			this->m_onGround = false;
	}

	if (this->m_velx > 0) {
		groundPlayerHitBox = { this->m_x + 1,this->m_y - 2, this->GetWidth(), this->GetHeight() };
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			if (content.tileMap[i]->CheckCollision(groundPlayerHitBox) && content.tileMap[i]->GetValue() > 0) {
				this->m_velx = 0;
				break;
			}
		}
	}

	if (this->m_velx < 0) {
		groundPlayerHitBox = { this->m_x - 1,this->m_y-2, this->GetWidth(), this->GetHeight() };
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			if (content.tileMap[i]->CheckCollision(groundPlayerHitBox) && content.tileMap[i]->GetValue() > 0) {
				this->m_velx = 0;
				break;
			}
		}
	}
	
	// When Player is jumping
	if (this->isJumping() && !this->isOnGround())
	{
		groundPlayerHitBox = { this->m_x,this->m_y + JUMP_STRENGTH, this->GetWidth(), this->GetHeight() };
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			if (content.tileMap[i]->CheckCollision(groundPlayerHitBox) && content.tileMap[i]->GetValue() > 0) {
				this->m_jumping = false;
				this->m_timer.Stop();
				// See if this can be moved to game class 
				if (content.tileMap[i]->GetValue() == TILE_ITEM)
				{
					if (this->m_pwrupState > 2)
						content.items.emplace_back(content.tileMap[i]->GetX()*TILE_WIDTH, content.tileMap[i]->GetY() * TILE_HEIGHT - TILE_HEIGHT, 3, graph);
					else
						content.items.emplace_back(content.tileMap[i]->GetX()*TILE_WIDTH, content.tileMap[i]->GetY() * TILE_HEIGHT - TILE_HEIGHT, this->m_pwrupState + 1, graph);
					content.tileMap[i]->SetValue(28);
					sound->PlaySound("uppop");
					
				}
				else if (content.tileMap[i]->GetValue() == TILE_COIN)
				{
					content.tileMap[i]->SetValue(28);
					this->m_score += 200;
					this->m_coins += 1;
					sound->PlaySound("coin");
				}
				else if (content.tileMap[i]->GetValue() == 2 && this->m_pwrupState > PLAYER_SMALL)
				{
					content.tileMap[i]->SetValue(0);
					sound->PlaySound("bricksmash");
				}
				break;
			}
		}	
	}

	// Test if a enemy is touched on fall
	if (!this->isOnGround())
	{
		groundPlayerHitBox = { this->m_x,this->m_y + 1, this->GetWidth(), this->GetHeight() };
		for (unsigned int i = 0; i < content.ennemies.size(); i++)
		{
			if (CheckCollision(groundPlayerHitBox, content.ennemies.at(i).GetRect()) && !this->isDead())
			{
				content.ennemies.erase(content.ennemies.begin() + i);
				printf("Enemy killed by Player\n");
				this->m_score += 100;
				sound->PlaySound("stomp");
			}
		}
	}
	
	for (unsigned int i = 0; i < content.ennemies.size(); i++)
		{
			if (CheckCollision(this->GetRect(), content.ennemies.at(i).GetRect()))
			{
				if (this->m_pwrupState == PLAYER_STAR)
				{
					content.ennemies.erase(content.ennemies.begin() + i);
					printf("Enemy killed by Player\n");
					this->m_score += 100;
					sound->PlaySound("stomp");
				}
				else
				{
					printf("Player Hit\n");
					this->Hit();
				}
			}
		}

	// See if this can be moved to game class 
	for (unsigned int i = 0; i < content.items.size(); i++)
	{		
		if (CheckCollision(this->GetRect(), content.items.at(i).GetRect()))
		{
			printf("Player : Item picked up\n");
			if (this->m_pwrupState == PLAYER_SMALL)
				this->m_y -= 16;
			this->m_pwrupState= content.items.at(i).PickUp();
			content.items.erase(content.items.begin() + i);
			sound->PlaySound("pwrup");
			this->m_score += 1000;			
		}
	}
		
	this->m_x += m_velx;
	this->m_y += m_vely;

	if (this->m_velx > 0)
		this->m_direction = false;
	else if (this->m_velx < 0)
		this->m_direction = true;

	// Limit the mouvement of the player to the boundaries of the level
	if (this->m_x > LEVEL_WIDTH - this->GetWidth())
		this->m_x = LEVEL_WIDTH - this->GetWidth();
	if (this->m_x < 0)
		this->m_x = 0;
	
	if (this->m_y < 0)
		this->m_y = 0;

	if (this->m_timer.isDone()) {
		this->m_jumping = false;
		this->m_timer.Stop();
	}

	// Gravity
	if (!this->m_onGround && !this->m_jumping)
		this->m_vely = -JUMP_STRENGTH;
	else if (!this->m_jumping)
		this->m_vely = 0;

	// Animations
	if (this->m_pwrupState == PLAYER_SMALL)
	{
		if (this->m_vely > 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "small_jump")
				this->m_sprite->PlayAnimation("small_jump");
		}
		else if (this->m_velx != 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "small_run")
				this->m_sprite->PlayAnimation("small_run");
		}
		else if (this->m_velx == 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "small_idle")
				this->m_sprite->PlayAnimation("small_idle");
		}
	}
	else if (this->m_pwrupState == PLAYER_GRAND)
	{
		if (this->m_vely > 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "grand_jump")
				this->m_sprite->PlayAnimation("grand_jump");
		}
		else if (this->m_velx != 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "grand_run")
				this->m_sprite->PlayAnimation("grand_run");
		}
		else if (this->m_velx == 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "grand_idle")
				this->m_sprite->PlayAnimation("grand_idle");
		}
	}
	else if (this->m_pwrupState >= PLAYER_FIRE)
	{
		if (this->m_vely > 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "fire_jump")
				this->m_sprite->PlayAnimation("fire_jump");
		}
		else if (this->m_velx != 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "fire_run")
				this->m_sprite->PlayAnimation("fire_run");
		}
		else if (this->m_velx == 0)
		{
			if (this->m_sprite->GetCurrentAnimation() != "fire_idle")
				this->m_sprite->PlayAnimation("fire_idle");
		}
	}

	if (this->GetY() > LEVEL_HEIGHT + 32)
		this->m_dead = true;
	
	this->m_sprite->Update();

	
}

void Player::Draw(Graphics* graph, int camX, int camY)
{
	if (m_sprite != NULL) {
		SDL_Rect sourceRect = GetOffset(this->m_pwrupState);
		SDL_Rect destRect = { this->m_x - camX, this->m_y - camY,sourceRect.w, sourceRect.h };
		if (this->m_direction)
			this->m_sprite->Draw(graph, &destRect,SDL_FLIP_HORIZONTAL);
		else
			this->m_sprite->Draw(graph, &destRect);
	}
}

void Player::setupAnimations()
{
	this->m_sprite->AddAnimation(1, 0, 0, 16, 16, "small_idle");
	this->m_sprite->AddAnimation(3, 16, 0, 16, 16, "small_run");
	this->m_sprite->AddAnimation(1, 80, 0, 16, 16, "small_jump");
	this->m_sprite->AddAnimation(1, 0, 16, 16, 32, "grand_idle");
	this->m_sprite->AddAnimation(3, 16, 16, 16, 32, "grand_run");
	this->m_sprite->AddAnimation(1, 80, 16, 16, 32, "grand_jump");
	this->m_sprite->AddAnimation(1, 0, 48, 16, 32, "fire_idle");
	this->m_sprite->AddAnimation(3, 16, 48, 16, 32, "fire_run");
	this->m_sprite->AddAnimation(1, 80, 48, 16, 32, "fire_jump");
}


void Player::Idle()
{
	this->m_velx = 0;
}

void Player::MoveLeft()
{
	if (this->m_sprint)
		this->m_velx = -(PLAYER_SPEED * 2);
	else
		this->m_velx = -(PLAYER_SPEED);
}

void Player::MoveRight()
{
	if (this->m_sprint)
		this->m_velx = PLAYER_SPEED*2;
	else
		this->m_velx = PLAYER_SPEED;
}

void Player::Jump(Sound* sound)
{
	if (this->isOnGround() && !this->isJumping()) {
		this->m_vely += JUMP_STRENGTH;
		this->m_jumping = true;
		this->m_timer.Start(300);
		sound->PlaySound("jump");
	}
}

void Player::Sprint()
{
	if (!this->m_sprint)
		this->m_sprint = true;
}

void Player::Unsprint()
{
	if (this->m_sprint)
		this->m_sprint = false;
}

void Player::Fire(Graphics * graph, std::vector<Bullet>* bullets)
{
	if (this->m_pwrupState >= PLAYER_FIRE)
		bullets->emplace_back(graph, this->m_x + 1, this->m_y + 16, 1);
}

void Player::Hit()
{
	if (this->m_pwrupState > PLAYER_SMALL)
	{
		this->m_pwrupState -= 1;
	}
	else
	{
		this->m_dead = true;
	}
	// else dead
}

void Player::Reset()
{
	this->m_velx = 0;
	this->m_vely = 0;
	this->m_onGround = false;
	this->m_jumping = false;
	this->m_direction = false;
	this->m_sprint = false;
	this->m_pwrupState = PLAYER_SMALL;
	this->m_dead = false;
}


bool Player::isOnGround()
{
	return this->m_onGround;
}

bool Player::isJumping()
{
	return this->m_jumping;
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
	return this->GetOffset(this->m_pwrupState).w;
}

int Player::GetHeight()
{
	return this->GetOffset(this->m_pwrupState).h;
}

int Player::GetScore()
{
	return this->m_score;
}

int Player::GetCoins()
{
	return this->m_coins;
}

SDL_Rect  Player::GetRect()
{
	SDL_Rect output = { this->m_x,this->m_y,this->GetWidth(),this->GetHeight() };
	return output;
}

void Player::SetX(int x)
{
	this->m_x = x;
}

void Player::SetY(int y)
{
	this->m_y = y;
}

SDL_Rect Player::GetOffset(int pwrup)
{
	SDL_Rect output;

	switch (pwrup)
	{
	case PLAYER_SMALL:
		output.w = 16;
		output.h = 16;
		output.x = 0;
		output.y = 0;
		break;
	case PLAYER_GRAND:
		output.w = 16;
		output.h = 32;
		output.x = 0;
		output.y = 16;
		break;
	case PLAYER_FIRE:
		output.w = 16;
		output.h = 32;
		output.x = 0;
		output.y = 48;
		break;
	default:
		output.w = 16;
		output.h = 32;
		output.x = 0;
		output.y = 16;
	}

	return output;
}

bool Player::isDead()
{
	return this->m_dead;
}
