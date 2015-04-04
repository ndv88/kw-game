#include "Weapon.h"
#include "Bullet.h"

extern BulletInfo bullets[];

Weapon::Weapon()
: GraphicObject()
, dir(0, 0)
{
	TYPE_ID = WEAPON_ID;

	bullets_id = 0;

	sprite = NULL;
	particle = NULL;

	deley = 0;
	timer = 0;

	firing = false;
}

Weapon::Weapon(hgeVector pos, short layer)
: GraphicObject(NULL, pos, 0, 0, layer)
, dir(0, 0)
{
	TYPE_ID = WEAPON_ID;

	bullets_id = 0;

	sprite = NULL;
	particle = NULL;

	deley = 0;
	timer = 0;

	firing = false;
}

Weapon::~Weapon()
{
	if (sprite)
	{
		delete sprite;
		sprite = NULL;
	}
	if (particle)
	{
		delete particle;
		particle = NULL;
	}
}

void Weapon::Initiate(WeaponInfo &info)
{
	bullets_id = info.bullets_id;
	deley = info.deley;

	if (0 < strlen(info.particle))
		particle = new hgeParticleSystem(*(resources_manager->GetParticleSystem(info.particle)));

	sprite = resources_manager->GetSprite( info.sprite );

	xw = info.xw;
	yw = info.yw;

	firing = true;
}

void Weapon::RotateTo(const hgeVector &taget)
{
	float rotation = 180/M_PI * atan2(taget.y, taget.x);

	hgeVector dist = taget - pos;
	dist.Normalize();

	dir += dist * rotation;
	dir.Normalize();
}

void Weapon::RotateTo(float x, float y)
{
	RotateTo(hgeVector(x, y));
}

void Weapon::Render()
{
	float x0, y0, x1, y1, x2, y2, x3, y3;

	float sint = (float)sqrt(1-dir.x * dir.x);
	float cost = dir.x;

	if (dir.y <= 0)
		sint = -sint;

	x0 = (-xw) * cost - (-yw) * sint + pos.x;
	y0 = (-xw) * sint + (-yw) * cost + pos.y;

	x1 = xw * cost - (-yw) * sint + pos.x; 
	y1 = xw * sint + (-yw) * cost + pos.y;

	x2 = xw * cost - yw * sint + pos.x;
	y2 = xw * sint + yw * cost + pos.y;

	x3 = (-xw) * cost - yw * sint + pos.x; 
	y3 = (-xw) * sint + yw * cost + pos.y;

	if (sprite)
	{
		sprite->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
	}

	if (particle)
		particle->Render();
}

void Weapon::Fire(float x, float y)
{
	if (firing)
	{
		RotateTo(x, y);

		float sint = (float)sqrt(1-dir.x * dir.x);
		float cost = dir.x;

		if (dir.y <= 0)
			sint = -sint;

		float x0 = xw * cost - (-(yw)) * sint + pos.x;
		float y0 = xw * sint + (-(yw)) * cost + pos.y;
		float x1 = xw * cost - yw * sint + pos.x;
		float y1 = xw * sint + yw * cost + pos.y;

		float _x = (x0+x1)/2;
		float _y = (y0+y1)/2;

		Bullet *bullet = new Bullet(hgeVector(_x, _y), layer);
		bullet->Initiate( bullets[bullets_id] );
		bullet->Fire(dir);
		bullet->InsertToWorld();

		firing = false;
		
		if (particle)
			particle->FireAt(pos.x, pos.y);
	}
}

void Weapon::Update()
{
	if (!firing)
	{
		timer += hge->Timer_GetDelta();
		if (timer >= deley)
		{
			firing = true;
			timer = 0;
		}
	}
}

void Weapon::UpdateGraphic()
{
	if (particle != NULL)
	{
		float sint = (float)sqrt(1-dir.x * dir.x);
		float cost = dir.x;

		if (dir.y <= 0)
			sint = -sint;

		float x0 = xw * cost - (-(yw)) * sint + pos.x;
		float y0 = xw * sint + (-(yw)) * cost + pos.y;
		float x1 = xw * cost - yw * sint + pos.x;
		float y1 = xw * sint + yw * cost + pos.y;

		float _x = (x0+x1)/2;
		float _y = (y0+y1)/2;

		float dt = hge->Timer_GetDelta();

		particle->Update(dt);
		particle->MoveTo(_x, _y);
	}
}