#pragma once


#include <fstream>

#include "CollisionDefines.h"
#include "GraphicObject.h"
#include "Edges.h"

#include <vector>

#define TILE_TYPE_EMPTY 0
#define TILE_TYPE_FULL 1

class Map;
class GraphicDynamicObject;

class TileMap: public GraphicObject
{
	friend Map;

public:
	TileMap();
	TileMap(char *sprite_name, hgeVector pos, short xw, short yw, short layer);

	virtual ~TileMap();

	virtual void SetTileType(short type);
	short GetTileType()											{ return tile_type; }

	virtual void Render();

	virtual void Save(std::ofstream *file);
	virtual void Load(std::ifstream *file);

	void InsertObject(GraphicDynamicObject *obj);
	void RemoveObject(GraphicDynamicObject *obj);

protected:
	short tile_type;

public:
	std::vector<GraphicDynamicObject *> objects;
	short edge_up, edge_down, edge_left, edge_right;

};
