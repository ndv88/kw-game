#pragma once

#include "GraphicObject.h"
#include "CollisionDefines.h"

#define MOVE_LEFT 1
#define	MOVE_RIGHT 2
#define	MOVE_UP 3
#define	MOVE_DOWN 4

class TileMap;
class World;

class GraphicDynamicObject: public GraphicObject
{
	friend World;
public:
	GraphicDynamicObject();
	GraphicDynamicObject(const char *texture_name, hgeVector pos, short xw, short yw, short layer);
	virtual ~GraphicDynamicObject();

	virtual void Verlet();
	virtual void ReportCollision(float px, float py, float dx, float dy, TileMap *tile);

	virtual void Move(int move);

	virtual void Render() = 0;

	void SetSpeed(float speed)						{ this->speed = this->max_speed = speed; }
	void SetSpeed(float speed, float max_speed)		{ this->speed = speed; this->max_speed = max_speed; }

	virtual int Update() = 0;

protected:
	hgeVector old_pos;
	float speed, max_speed;

	TileMap *tile;
	//TileMap *prev_tile;
};
