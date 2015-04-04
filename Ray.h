#pragma once

#include <hgevector.h>

class GraphicDynamicObject;
class AABB;
class TileMap;
class TileMapEx;
class Map;

struct CollideWorldInfo
{
	GraphicDynamicObject *object;
	TileMap *tile;
};

class Ray
{
public:
	static bool QueryRayObject(hgeVector &result, hgeVector &p0, hgeVector &p1, GraphicDynamicObject *obj);
	static bool QueryRayVsWorld(hgeVector &p0, hgeVector &p1);

	static bool TestRayObject(hgeVector &result, float px, float py, float dx, float dy, GraphicDynamicObject *obj);
	static bool TestRayVsAABB(hgeVector &result, float px, float py, float dx, float dy, AABB *aabb);

	static bool CollideRayVsTiles(hgeVector &result, int layer, hgeVector &p0, hgeVector &p1);
	static bool TestRayVsTile(hgeVector &result, float px, float py, float dx, float dy, TileMap *t);
	static bool TestRayVsTileFull(hgeVector &result, float px, float py, float dx, float dy, TileMap *t);
	static bool TestRayVsTile45Degree(hgeVector &out, float px, float py, float dx, float dy, TileMapEx *t);

	static bool CollideRayVsWorld(hgeVector &result, hgeVector &p0, hgeVector &p1, short layer, CollideWorldInfo &info);

public:
	static Map *tiles;
};
