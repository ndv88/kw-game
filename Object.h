#pragma once

#include "System.h"

#define OBJECT_ID 1
#define GRAPHIC_OBJECT_ID 2
#define GRAPHIC_DYNAMIC_OBJECT_ID 10

#define AABB_ID 11
#define ENTITY_ID 12

#define TILE_MAP_ID 20
#define TILE_MAP_EXTENDED_ID 21
#define TILE_MAP_ANIMATION_ID 22
#define TILE_MAP_PARTICLE_ID 23
#define TILE_MAP_ANIMATION_EX_ID 24
#define TILE_MAP_RESOURCE_ID 25

#define MAP_ID 34

#define WORLD_ID 44
#define BACKGROUND_ID 45

#define WEAPON_ID 340

#define BULLET_ID 440


class Object
{
public:
	Object();
	virtual ~Object();

	virtual inline int GetObjectType() const
	{ return TYPE_ID; }

protected:
	short TYPE_ID;
};
