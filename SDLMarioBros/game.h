#pragma once

#include <stdio.h>

#include <fstream>

#include <SDL2\SDL.h>

#include "graphics.h"
#include "player.h"
#include "tile.h"

class Game
{
public:
	Game();
	~Game();
	int SDLInit();
	void GameLoop();
	void Draw();		// Draw call for the graphics objet to render
	void Update();		// Update call for the game logic 
	void HandleInput(const Uint8 *keystate);
	bool loadTileMap(Texture* tileset);

private:
	Graphics* m_graphics;
	Player* m_player;
	SDL_Rect camera;
	Tile* m_tileMap[TOTAL_TILES];
};
