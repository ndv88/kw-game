#include "Bullet.h"
#include "Entity.h"
#include "World.h"
#include "Ray.h"



Bullet::Bullet()
: GraphicDynamicObject()
, dir(0, 0)
, power(0)
, flight_ps(NULL)
, explode_ps(NULL)
{
	TYPE_ID = BULLET_ID;
	state_id = WAIT;

	start_accel = rate_accel = current_accel = 0;
}

Bullet::Bullet(hgeVector pos, short layer)
: GraphicDynamicObject(NULL, pos, 0, 0, layer)
, dir(0, 0)
, power(0)
, flight_ps(0)
, explode_ps(NULL)
{
	TYPE_ID = BULLET_ID;

	state_id = WAIT;

	start_accel = rate_accel = current_accel = 0;
}

Bullet::~Bullet()
{
	if (flight_ps)
		delete flight_ps;
	if (explode_ps)
		delete explode_ps;
}

void Bullet::Initiate(BulletInfo &info)
{
	power = info.power;

	xw = info.xw;
	yw = info.yw;

	start_accel = info.start_accel;
	rate_accel = info.rate_accel;
	current_accel = start_accel;

	speed = info.speed;
	max_speed = info.max_speed;

	resource_name = info.sprite;
	flight_ps = new hgeParticleSystem(*(resources_manager->GetParticleSystem(info.flight_particle_name)));
	explode_ps = new hgeParticleSystem(*(resources_manager->GetParticleSystem(info.explode_particle_name)));
}

void Bullet::Render()
{
	if (EXPLODE == state_id)
	{
		explode_ps->Render();
	}
	else if (FLIGHT == state_id)
	{
		float x0, y0, x1, y1, x2, y2, x3, y3;

		x0 = pos.x - xw; y0 = pos.y - yw;
		x1 = pos.x + xw; y1 = pos.y - yw;
		x2 = pos.x + xw; y2 = pos.y + yw;
		x3 = pos.x - xw; y3 = pos.y + yw;

		if (!resource_name.empty())
		{
			hgeSprite *sprite = resources_manager->GetSprite(resource_name.c_str());

			if (sprite)
			{
				sprite->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
			}
		}

		//hge->Gfx_RenderLine(pos.x, pos.y, taget.x, taget.y);

		flight_ps->Render();
	}

}

void Bullet::Fire(hgeVector dir)
{
	state_id = FLIGHT;
	flight_ps->FireAt(pos.x, pos.y);

    current_accel = start_accel;
    speed = 0;

	this->dir = dir;

    //float dx = x - pos.x;
    //float dy = y - pos.y;

    //float l = sqrt(dx*dx + dy*dy);

    //if (l != 0)
    //{
    //    dx = dx / l;
    //    dy = dy / l;
    //    dir.x = dx;
    //    dir.y = dy;
    //}
}

void Bullet::Flight()
{
	if (FLIGHT == state_id)
	{
		if (speed < max_speed) 
		{
			current_accel = current_accel * rate_accel;
			speed = speed + current_accel;
		}
		else 
		{
			speed = max_speed;
		}

		old_pos = pos;

		pos.x = pos.x + speed * dir.x;
		pos.y = pos.y + speed * dir.y;

		if (world->GetMap()->QueryPoint(layer, pos.x, pos.y))
		{
			Explode();
			return;
		}

		hgeVector temp;
		CollideWorldInfo info;

		Ray::CollideRayVsWorld(temp, old_pos, pos, layer, info);

		//taget = temp;
		
		if (info.object != NULL)
		{
			TestVsEntity((Entity *)info.object);
		}
	}
}

void Bullet::Explode()
{
	state_id = EXPLODE;

	flight_ps->Stop();

	explode_ps->FireAt(pos.x, pos.y);
}

void Bullet::TestVsEntity(Entity *guy)
{
	if (guy)
	{
		hgeVector guy_pos = guy->GetPos();

		//float dx = guy_pos.x - pos.x;
		//float dy = guy_pos.y - pos.y;

		float r = 32;

		//float l = sqrt(dy*dy + dx*dx);

		//hge->System_Log("l = %f, r = %f", l, r);

		hgeRect rect(guy_pos.x-r,guy_pos.y-r, guy_pos.x+r, guy_pos.y+r);

		if (rect.TestPoint(pos.x, pos.y)) //if (l < r)
		{
			guy->KillEntity(this);

			//old_pos = pos;

			//pos.x += dx;
			//pos.y += dy;

			Explode();
		}
	}
}

void Bullet::UpdateGraphic()
{
	float dt = hge->Timer_GetDelta();

	if (EXPLODE == state_id)
	{
		explode_ps->Update(dt);
	}
	else if (FLIGHT == state_id)
	{
		flight_ps->MoveTo(pos.x, pos.y);
		flight_ps->Update(dt);
	}
}

int Bullet::Update()
{
	if (FLIGHT == state_id)
	{
		Flight();
	}
	else if (EXPLODE == state_id)
	{
		if (0 == explode_ps->GetParticlesAlive())
		{
			state_id = NONE;
		}
	}

	return (int)state_id;
}