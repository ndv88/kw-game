#pragma once

#include <vector>

#include "Object.h"
#include "Map.h"
#include "Entity.h"
#include "Background.h"


class Bullet;


class World: public Object
{
public:	
	World();
	virtual ~World();

	void Create(Map *map, size_t world_layers);
	void Free();

	Map* GetMap() const					{ return map; }
	size_t GetLayers() const				{ return layers; }

	void SetPlayer(Entity *player)			{ this->player = player; }
	Entity *GetPlayer()						{ return player; }
	void RemovePlayer()						{ if (player) {delete player; player = NULL;} }

	void InsertObject(GraphicObject *obj);
	void RemoveObject(GraphicObject *obj);

	void UpdateObjects();
	void UpdatePositionObjects();//синхронизовать со скорость игры
	void UpdateGraphic();
	
	std::vector<Entity *> *GetWorldEntitys() { return &entitys; }

	void Render();

	virtual void Reset();

	void SetWorldOffsetX(float offset) { map_offset_x = offset; }
	void SetWorldOffsetY(float offset) { map_offset_y = offset; }

protected:
	Entity *player;

	std::vector<Bullet *> bullets;
	std::vector<Entity *> entitys;

	size_t layers;

	float map_offset_x, map_offset_y;
	Map *map;

	Background *bg;
};
