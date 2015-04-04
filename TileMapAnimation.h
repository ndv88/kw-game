#pragma once

#include <hgeanim.h>

#include "TileMap.h"

class TileMapAnimation: public TileMap
{
public:
	TileMapAnimation();
	TileMapAnimation(char *res_name, hgeVector pos, short xw, short yw, short layer);
	virtual ~TileMapAnimation();

	virtual void Render();

	virtual void Save(std::ofstream *file);
	virtual void Load(std::ifstream *file);

	virtual void UpdateGraphic();

	virtual void Play();
	virtual void Stop();

protected:
	hgeAnimation *animation;
};
