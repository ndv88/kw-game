#pragma once

#include "TileMapAnimation.h"
#include "TileMapParticle.h"

class TileMapAnimationEx: public TileMapAnimation
{
public:
	TileMapAnimationEx();
	TileMapAnimationEx(char *animation_name, char *particle_name, hgeVector pos, short xw, short yw, short layer);
	virtual ~TileMapAnimationEx();

	virtual void Render();

	virtual void Save(std::ofstream *file);
	virtual void Load(std::ifstream *file);

	virtual void UpdateGraphic();

	hgeParticleSystem * GetParticle() const							{ return ps; }

	virtual void Play();
	virtual void Stop();
	virtual void MoveParticle(float x, float y, bool move);

	const char * GetPSName()										{ return ps_name.c_str(); }

protected:
	hgeParticleSystem *ps;
	std::string ps_name;
};
