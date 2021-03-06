#pragma once

#include <vector>

#include <SDL2\SDL.h>

#include "item.h"
#include "tile.h"
#include "enemy.h"
#include "bullet.h"

struct LevelContent
{
	Tile* tileMap[TOTAL_TILES];
	std::vector<Enemy> ennemies;
	std::vector<Item> items;
	std::vector<Bullet> bullets;
};

void UpdateContent(LevelContent* m_content, SDL_Rect camera);
void DrawContent(LevelContent* m_content, Graphics* graph, SDL_Rect* camera);
void ClearTilemap(Tile* tilemap[]);
void ClearContent(LevelContent* m_content);

