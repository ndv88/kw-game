#pragma once

#include "GraphicObject.h"

struct WeaponInfo
{
	int bullets_id;

	short xw, yw;

	float deley;
	char *particle;
	char *sprite;
};

class Weapon: public GraphicObject
{
	Weapon();
public:
	Weapon(hgeVector pos, short layer);
	virtual ~Weapon();

	void Initiate(WeaponInfo &info);

	void RotateTo(const hgeVector &taget);
	void RotateTo(float x, float y);

	virtual void Render();

	virtual void Fire(float x, float y);

	virtual void Update();
	virtual void UpdateGraphic();

	void SetDir(hgeVector dir) { this->dir = dir; }

protected:
	hgeVector dir;

	int bullets_id;

	hgeParticleSystem *particle;
	hgeSprite *sprite;

	float deley;
	float timer;

	bool firing;
};
