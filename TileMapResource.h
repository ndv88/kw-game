#pragma once

#include "TileMapAnimationEx.h"

enum RESOURCE_TYPES
{
	LIFES = 1,
	AID = 2,
	ARMOUR = 3
};

class Entity;

class TileMapResource: public TileMapAnimationEx
{
public:
	TileMapResource();
	TileMapResource(char *animation_name, char *particle_name, char *particle_used_name, hgeVector pos, short xw, short yw, short layer);
	~TileMapResource();

	inline void SetResourceType(int type)		{ resource_type = type; }
	inline int GetResourceType() const			{ return resource_type; }
	
	inline bool IsUsed() const					{ return used; }
	virtual void Used(bool used);
	virtual void UpdateGraphic();
	
	virtual void Render();

	void MoveParticle(float x, float y, bool move);

	virtual void Save(std::ofstream *file);
	virtual void Load(std::ifstream *file);

	const char* GetUsedPSName()					{ return used_name_ps.c_str(); }

protected:
	int resource_type;

	bool used;

	float time1;
	float time2;

	hgeParticleSystem *ps_used;
	std::string used_name_ps;
};
