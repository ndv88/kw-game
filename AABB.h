#pragma once

#include <hgevector.h>
#include "GraphicDynamicObject.h"

class TileMap;
class TileMapEx;
class Map;

class AABB: public GraphicDynamicObject
{
public:
	AABB();
	AABB(const char *texture_name, hgeVector pos, short xw, short yw, short layer);
	virtual ~AABB();

	virtual void Render();

	void CollideVsTile(TileMap *tile);
	void CollideVsTileMap(Map *map);

	virtual int Update();

	float world_offset_x;
	float world_offset_y;

protected:
	virtual int Resolve(float x, float y, TileMap *t);
	int CollideVsTileFull(float x, float y, TileMap *t);
	int CollideVsTile45Degree(float x, float y, TileMapEx *t);
	int CollideVsTile22Degree(float x, float y, TileMapEx *t);
};
