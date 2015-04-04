#pragma once

#include "TileMap.h"

#define TILE_TYPE_45DEGREE_pn 2//45-degree triangle, whose normal is (+ve,-ve)
#define TILE_TYPE_45DEGREE_nn 3//(+ve,+ve)
#define TILE_TYPE_45DEGREE_np 4//(-ve,+ve)
#define TILE_TYPE_45DEGREE_pp 5//(-ve,-ve)

#define TILE_TYPE_22DEGREE_pn 6
#define TILE_TYPE_22DEGREE_nn 7
#define TILE_TYPE_22DEGREE_np 8
#define TILE_TYPE_22DEGREE_pp 9


class TileMapEx: public TileMap
{
public:
	TileMapEx();
	TileMapEx(char *sprite_name, hgeVector pos, short xw, short yw, short layer);
	virtual ~TileMapEx();

	virtual void SetTileType(short type);

	virtual void Save(std::ofstream *file);
	virtual void Load(std::ifstream *file);

	virtual void Render();

public:
	float signx, signy;
	float sx, sy;
};
