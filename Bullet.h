#pragma once

#include "GraphicDynamicObject.h"

enum BulletState
{
	WAIT = 1,
	FLIGHT = 2,
	EXPLODE = 3,
	NONE = 0
};

struct BulletInfo
{
	float power;

	short xw, yw;

	float start_accel;
    float rate_accel;
	float speed;
	float max_speed;
	char *sprite;
	char *flight_particle_name;
	char *explode_particle_name;
};

class Entity;

class Bullet: public GraphicDynamicObject
{
public:
	Bullet();
	Bullet(hgeVector pos, short layer);
	virtual ~Bullet();

	void Initiate(BulletInfo &info);

	inline float GetPower() const				{ return power; }

	virtual void Render();

	virtual void Fire(hgeVector dir);
	virtual void Flight();
	virtual void Explode();

	virtual void TestVsEntity(Entity *guy);

	virtual void UpdateGraphic();
	virtual int Update();

protected:

	//hgeVector taget;

	int state_id;
	hgeVector dir;

	hgeParticleSystem *flight_ps;
	hgeParticleSystem *explode_ps;

	float power;

	float start_accel;
    float rate_accel;
	float current_accel;
};
