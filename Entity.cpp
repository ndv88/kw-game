#include "Entity.h"
#include "Bullet.h"
#include "Weapon.h"

#include "World.h"

#include "Ray.h"

#include "TileMapResource.h"

extern WeaponInfo weapons[];
extern EntityInfo entitys[];

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

Entity::Entity(hgeVector pos, short layer)
: AABB(NULL, pos, 0, 0, layer)
{
	TYPE_ID = ENTITY_ID;

	id = -1;

	skin_id = -1;

	weapon_id = -1;

	stand = NULL;
	run = NULL;
	jump = NULL;
	fall = NULL;
	skid = NULL;
	death = NULL;
	current_animation = NULL;

	life = 100;
	armour = 0;

	weapon = NULL;

	state_id = ENTITY_NONE_STATE;
	Stand();

	in_air = true;
	in_run = false;

	face_direction = false;

	friction_run = 0;
	friction_skid = 0;

	jump_force = 0;
	jump_bias = 0;

	target.x = 0;
	target.y = 0;
	attack_flag = false;

	is_disconnect = false;

	timer = 0; 
}

Entity::~Entity()
{
}

void Entity::Verlet()
{
	float d = DRAGGING;
	float g = GRAVITATION;
	
	float px, py;
		
	float ox = old_pos.x; //we can't swap buffers since mcs/sticks point directly to vector2s..
	float oy = old_pos.y;

	old_pos.x = px = pos.x;		//get vector values
	old_pos.y = py = pos.y;		//pos = position  
								//old_pos = old position

	//integrate	
	pos.x += (d*px) - (d*ox);
	pos.y += (d*py) - (d*oy) + g;
}


void Entity::Initiate(int skin_id)
{
	this->skin_id = skin_id;

	EntityInfo &info = entitys[skin_id];
	
	life = info.life;
	armour = info.armour;

	face_direction = info.face_direction;

	xw = info.xw;
	yw = info.yw;

	SetWeapon(info.weapon_id);

	friction_run = info.friction_run;
	friction_skid = info.friction_skid;

	jump_force = info.jump_force;
	jump_bias = info.jump_bias;

	stand = new hgeAnimation( *(resources_manager->GetAnimation( info.stand )) );
	jump = new hgeAnimation( *(resources_manager->GetAnimation( info.jump )) );
	run = new hgeAnimation( *(resources_manager->GetAnimation( info.run )) );
	fall = new hgeAnimation( *(resources_manager->GetAnimation( info.fall )) );
	skid = new hgeAnimation( *(resources_manager->GetAnimation( info.skid )) );
	death = new hgeAnimation( *(resources_manager->GetAnimation( info.death )) );

	state_id = ENTITY_NONE_STATE;
	Stand();
	current_animation = stand;

} 

void Entity::SetWeapon(int weapon_id)
{
	if (weapon)
	{
		delete weapon;
		weapon = NULL;
	}

	this->weapon_id = weapon_id;

	weapon = new Weapon(pos, layer);
	weapon->Initiate(weapons[weapon_id]); 
}

void Entity::ReportCollision(float px, float py, float dx, float dy, TileMap *tile)
{

	if (tile->GetObjectType() == TILE_MAP_RESOURCE_ID)
	{
		if (dx < 0 || dy < 0)
		{
			UseResource((TileMapResource *)tile);
			return;
		}
	}

	if (dy < 0)
	{
		if (in_air)
		{
			//play sound
			in_air = false;
		}
	}
/*
	if (0.8*r*r < px*px+py*py) 
	{
		// game over
		// return;
	}
*/
	float vx = pos.x - old_pos.x;
	float vy = pos.y - old_pos.y;
	
	float dp = (vx*dx + vy*dy);
	float nx = dp*dx;
	
	float ny = dp*dy;
	
	float tx = vx-nx;
	float ty = vy-ny;

	float b,bx,by,f,fx,fy;
	if(dp < 0)
	{
		if (state_id == ENTITY_RUNNING)
		{
			f = friction_run;
		}
		else if (state_id == ENTITY_SKIDDING)
		{
			f = friction_skid;
		}
		else
		{
			f = (float)FRICTION;
		}

		fx = tx * f;
		fy = ty * f;
		
		b = 1;

		bx = (nx * b);
		by = (ny * b);
	}
	else
	{
		bx = by = fx = fy = 0;
	}

	pos.x += px;
	pos.y += py;
	
	old_pos.x += px + bx + fx;
	old_pos.y += py + by + fy;
}

void Entity::Move(int move)
{
	if (MOVE_RIGHT == move)
	{
		Run();
		// face_direction = 0;

		int current_mode = current_animation->GetMode();

		if (face_direction == 0 && current_mode & HGEANIM_FWD)
		{
			current_animation->SetMode(HGEANIM_FWD|HGEANIM_LOOP);
		}
		else if (face_direction == 1 && current_mode & HGEANIM_REV)
		{
			current_animation->SetMode(HGEANIM_REV|HGEANIM_LOOP);
		}

		AABB::Move(move);
	}
	else if (MOVE_LEFT == move)
	{
		Run();
		// face_direction = 1;

		int current_mode = current_animation->GetMode();

		if (face_direction == 0 && current_mode & HGEANIM_REV)
		{
			current_animation->SetMode(HGEANIM_REV|HGEANIM_LOOP);
		}
		else if (face_direction == 1 && current_mode & HGEANIM_FWD)
		{
			current_animation->SetMode(HGEANIM_FWD|HGEANIM_LOOP);
		}

		AABB::Move(move);
	}
}

void Entity::Render()
{
	float x0, y0, x1, y1, x2, y2, x3, y3;

	x0 = pos.x - xw; y0 = pos.y - yw;
	x1 = pos.x + xw; y1 = pos.y - yw;
	x2 = pos.x + xw; y2 = pos.y + yw;
	x3 = pos.x - xw; y3 = pos.y + yw;

	if (current_animation)
	{
		weapon->SetPos(pos);
		weapon->Render();

		current_animation->SetFlip(face_direction, false);
		current_animation->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
	}
}

int Entity::Update()
{
	AABB::Update();

	float vx = pos.x - old_pos.x;
	float vy = pos.y - old_pos.y;
	
	float dy = pos.y - old_pos.y;

	if (state_id == ENTITY_DEATH)
	{
		Death();
	}
	else if (0 < dy)
	{
		in_air = true;
		Fall();
	}
	//else if (in_air)
	//{
	//	if (0 < dy)
	//	{
	//		Fall();
	//	}
	//}
	else if (state_id == ENTITY_RUNNING)
	{
		float dx = pos.x - old_pos.x;

		if (0 == dx)
		{
			Stand();
		}
		else if (!in_run)
		{
			Skid();
		}
		else
		{
			Run();
		}

		in_run = false;
	}
	else
	{
		int dx = (int)pos.x - old_pos.x;

		if (0 == dx)
		{
			Stand();
		}
	}

	TestVsResource();

	weapon->SetPos(pos);
	weapon->Update();

	if (state_id == ENTYTY_STATE::ENTITY_DEATH)
	{
		timer += hge->Timer_GetDelta();

		if (timer >= 0.200)
		{
			return state_id;
		}
		else
		{
			return (int)ENTITY_NONE_STATE;
		}
	}

	return state_id;
}

int Entity::UpdateIsNetworkGame()
{
	float vx = pos.x - old_pos.x;
	float vy = pos.y - old_pos.y;
	
	float dy = pos.y - old_pos.y;

	if (state_id == ENTITY_DEATH)
	{
		Death();
	}
	else if (0 < dy)
	{
		in_air = true;
		Fall();
	}
	//else if (in_air)
	//{
	//	if (0 < dy)
	//	{
	//		Fall();
	//	}
	//}
	else if (state_id == ENTITY_RUNNING)
	{
		float dx = pos.x - old_pos.x;

		if (0 == dx)
		{
			Stand();
		}
		else if (!in_run)
		{
			Skid();
		}
		else
		{
			Run();
		}

		in_run = false;
	}
	else
	{
		int dx = (int)pos.x - old_pos.x;

		if (0 == dx)
		{
			Stand();
		}
	}

	TestVsResource();

	weapon->SetPos(pos);
	weapon->Update();

	if (tile)
	{
		tile->RemoveObject(this);
		tile = NULL;
	}

	world_offset_x = world->GetMap()->GetOffsetX();
	world_offset_y = world->GetMap()->GetOffsetY();

	//Verlet();
	CollideVsTileMap(world->GetMap());

	weapon->SetPos(pos);
	weapon->Update();

	return state_id;
}

void Entity::UpdateGraphic()
{
	float dt = hge->Timer_GetDelta();
	current_animation->Update(dt);
}

void Entity::Think()
{

	if (state_id != ENTITY_DEATH)
	{
		hgeVector p0 = this->pos;
		hgeVector p1 = world->GetPlayer()->GetPos();

		float _x = abs(abs(p0.x)-abs(p1.x));
		float _y = abs(abs(p0.y)-abs(p1.y));
	
		if ((_x >= 0 && _x <= SCREEN_WIDTH/2) && (_y >= 0 && _y <= SCREEN_HEIGHT/2))
		{
			hgeVector out(0, 0);

			if (Ray::QueryRayVsWorld(p0, p1))
			{
				this->FaceToTarget(p1);
				this->Attack(p1);
			}
			else
			{
				if (weapon)
					weapon->SetDir(hgeVector(0, 0));
			}
		}
		else
		{
			if (weapon)
				weapon->SetDir(hgeVector(0, 0));
		}


		if (!in_air)
		{
			int row = 0, col = 0;
			world->GetMap()->GetTileIndex(&pos, row, col);
			TileMap *tile = world->GetMap()->GetTile(layer, row, col);
			TileMap *neighbor = NULL;
			if (face_direction == 0)
			{
				neighbor = world->GetMap()->GetTile(layer, row, col+1);
				if (tile->edge_right != EDGE_ID_SOLID && neighbor->edge_down != EDGE_ID_OFF)
				{
					Move(MOVE_RIGHT);
				}
				else
				{
					face_direction = 1;
					return;
				}
			}
			else if (face_direction == 1)//left
			{
				neighbor = world->GetMap()->GetTile(layer, row, col-1);
				if (tile->edge_left != EDGE_ID_SOLID && neighbor->edge_down != EDGE_ID_OFF)
				{
					Move(MOVE_LEFT);
				}
				else
				{
					face_direction = 0;
					return;
				}
			}
		}
	}
/**/
}

void Entity::FaceToTarget(hgeVector &v)
{
	if (pos.x < v.x)
	{
		face_direction = 0;
	}
	else if (pos.x > v.x)
	{
		face_direction = 1;
	}
}

void Entity::Stand()
{
	if (!in_air && state_id != ENTITY_STANDING)
	{
		ExitState();
		state_id = ENTITY_STANDING;

		stand->Play();
		current_animation = stand;
	}
}

void Entity::EndStand()
{
	stand->Stop();
	stand->SetFrame(0);
	current_animation = NULL;
}

void Entity::Run()
{
	in_run = true;

	if (!in_air && state_id != ENTITY_RUNNING)
	{
		ExitState();
		state_id = ENTITY_RUNNING;

		run->Play();
		current_animation = run;
	}
}

void Entity::EndRun()
{
	run->Stop();
	run->SetFrame(0);
	current_animation = NULL;
}

void Entity::Jump()	//void Entity::Jump(float x, float y)
{
	if (!in_air && state_id != ENTITY_JUMPING)
	{
		ExitState();

		in_air = true;
		state_id = ENTITY_JUMPING;
		//gravitation = jump_gravitation;

		float vx = pos.x - old_pos.x;
		float vy = pos.y - old_pos.y;
/*
		if (vx * x < 0)
		{
			old_pos.x = pos.x;
		}
		if (vy * y < 0)
		{
			old_pos.y = pos.y;
		}
*/
		old_pos.x = pos.x;
		old_pos.y = pos.y;
/*
		pos.x = pos.x + x * jump_force;
		pos.y = pos.y + y * (jumpAmt + jump_bias);
*/
		//pos.x = pos.x + vx;
		//pos.y = pos.y * (jump_force + jump_bias);
		pos.y = pos.y - (jump_force + jump_bias);

		jump->Play();
		current_animation = jump;
	}
}

void Entity::EndJump()
{
    //gravitation = normal_gravitation;
	jump->Stop();
	jump->SetFrame(0);
	current_animation = NULL;
}

void Entity::Fall()
{
	if (in_air && state_id != ENTITY_FALLING)
	{
		ExitState();

		//in_air = true;
		state_id = ENTITY_FALLING;

		fall->Play();
		current_animation = fall;

	}
}

void Entity::EndFall()
{
	fall->Stop();
	fall->SetFrame(0);
	current_animation = NULL;
}

void Entity::Skid()
{
	if (!in_air && state_id != ENTITY_SKIDDING)
	{
		ExitState();
		state_id = ENTITY_SKIDDING;

		skid->Play();
		current_animation = skid;
	}

}

void Entity::EndSkid()
{
	skid->Stop();
	skid->SetFrame(0);
	current_animation = NULL;
}

void Entity::Death()
{
	if (state_id != ENTITY_DEATH)
	{
		ExitState();
		state_id = ENTITY_DEATH;

		if (tile)
		{
			tile->RemoveObject(this);
			tile = NULL;
		}

		death->Play();
		current_animation = death;
	}
}

void Entity::ExitState()
{
	switch (state_id)
	{
	case ENTITY_STANDING:
		EndStand();
		break;
	case ENTITY_RUNNING:
		EndRun();
		break;
	case ENTITY_SKIDDING:
		EndSkid();
		break;
	case ENTITY_JUMPING:
		EndJump();
		break;
	case ENTITY_FALLING:
		EndFall();
		break;
	}
}

void Entity::Attack(hgeVector &target)
{	
	this->target = target;
	this->attack_flag = true;

	weapon->Fire(target.x, target.y);
}

void Entity::RotateWeapon(hgeVector &target)
{
	this->target = target;

	weapon->RotateTo(target);
}

void Entity::KillEntity(Bullet *bullet)
{
	life -= bullet->GetPower();
	if (life <= 0)
	{
		Death();
	}
}

int Entity::Resolve(float x, float y, TileMap *t)
{
	if (t->GetObjectType() != TILE_MAP_RESOURCE_ID)
		return AABB::Resolve(x, y, t);
	else
	{
		UseResource((TileMapResource *)t);
		return COLLISION_NONE;
	}
}

void Entity::UseResource(TileMapResource *t)
{
	if (!t->IsUsed() && this == world->GetPlayer())
	{
		bool used = false;
		switch (t->GetResourceType())
		{
		case RESOURCE_TYPES::AID:
			if (life < 100)
			{
				int aid = 15;
				life = life + aid;

				if (life > 100)
					life = life - (life % 100);

				used = true;
			}
			break;
		case RESOURCE_TYPES::LIFES:

			if (life < 100)
			{
				int aid = 25;
				life = life + aid;

				if (life > 100)
					life = life - (life % 100);

				used = true;
			}

			break;
		case RESOURCE_TYPES::ARMOUR:

			break;
		}
		t->Used(used);
	}
}

void Entity::GetEntityData(ENTITY_NET_DATA &data)
{
	Map *map = world->GetMap();


	data.id = this->id;
	data.pos = this->pos;
	data.old_pos = this->old_pos;

	data.state_id = this->state_id;
	data.skin_id = this->skin_id;
	data.weapon_id = this->weapon_id;
	
	data.in_run = this->in_run;
	data.in_air = this->in_air;
	data.face_direction = this->face_direction;
	
	
	data.life = this->life;
	data.armour = this->armour;

	data.world_offset_x = map->GetOffsetX();
	data.world_offset_y = map->GetOffsetY();

	data.is_disconnect = this->is_disconnect;

	data.target = this->target;
	data.attack = this->attack_flag;
}

void Entity::SetEntityData(ENTITY_NET_DATA &data)
{
	this->id = data.id;
	this->pos = data.pos;
	this->old_pos = data.old_pos;

	//this->state_id = data.state_id;
	this->Initiate(data.skin_id);
	//SetWeapon(data.weapon_id);

	//if (state_id != data.state_id)
	//{
	//	ExitState();
	//}
		
		//this->state_id = data.state_id;

		this->in_run = data.in_run;
		this->in_air = data.in_air;

		this->face_direction = data.face_direction;

		switch (state_id)
		{
		case ENTITY_STANDING:
			Stand();
			break;
		case ENTITY_RUNNING:
			Run();
			break;
		case ENTITY_SKIDDING:
			Skid();
			break;
		case ENTITY_JUMPING:
			Jump();
			break;
		case ENTITY_FALLING:
			Fall();
			break;
		}
	//else
	//{
	//	this->in_run = data.in_run;
	//	this->in_air = data.in_air;

	//	this->face_direction = data.face_direction;
	//}


	
	this->life = data.life;
	this->armour = data.armour;
	
	this->world_offset_x = data.world_offset_x;
	this->world_offset_y = data.world_offset_y;

	this->is_disconnect = data.is_disconnect;

	if (data.attack)
		Attack(data.target);
	else
		RotateWeapon(data.target);

	if (weapon)
		weapon->SetPos(this->pos);
}

void Entity::GetEntityData(ENTITY_NET_DATA_SMALL &data)
{
	Map *map = world->GetMap();

	data.id = this->id;
	data.pos = this->pos;
	data.old_pos = this->old_pos;

	data.state_id = this->state_id;

	data.in_run = this->in_run;
	data.in_air = this->in_air;
	data.face_direction = this->face_direction;

	data.world_offset_x = map->GetOffsetX();
	data.world_offset_y = map->GetOffsetY();
	
	data.is_disconnect = this->is_disconnect;

	data.target = this->target;
	data.attack = this->attack_flag;
}

void Entity::SetEntityData(ENTITY_NET_DATA_SMALL &data)
{
	this->id = data.id;
	this->pos = data.pos;
	this->old_pos = data.old_pos;

	//if (state_id != data.state_id)
	//{
	//	ExitState();
	//}
		
		//this->state_id = data.state_id;

		this->in_run = data.in_run;
		this->in_air = data.in_air;

		this->face_direction = data.face_direction;

		switch (state_id)
		{
		case ENTITY_STANDING:
			Stand();
			break;
		case ENTITY_RUNNING:
			Run();
			break;
		case ENTITY_SKIDDING:
			Skid();
			break;
		case ENTITY_JUMPING:
			Jump();
			break;
		case ENTITY_FALLING:
			Fall();
			break;
		}
	//else
	//{
	//	this->in_run = data.in_run;
	//	this->in_air = data.in_air;

	//	this->face_direction = data.face_direction;
	//}

	this->in_run = data.in_run;
	this->in_air = data.in_air;
	this->face_direction = data.face_direction;

	this->world_offset_x = data.world_offset_x;
	this->world_offset_y = data.world_offset_y;

	this->is_disconnect = data.is_disconnect;

	if (data.attack)
		Attack(data.target);
	else
		RotateWeapon(data.target);

	if (weapon)
		weapon->SetPos(this->pos);
}

void Entity::Reset()
{
	this->attack_flag = false;
}

bool Entity::IsDisconnect()
{
	if (is_disconnect)
	{
		if (tile)
		{
			tile->RemoveObject(this);
			tile = NULL;
		}
	}

	return is_disconnect;
}

void Entity::TestVsResource()
{
	Map *map = world->GetMap();

	hgeVector p;
	TileMap *t = NULL;

	for (int i = 0; i < 5; i++)
	{
		p = pos;
		t = NULL;
		
		switch(i)
		{
		case 1:
			p.x -= xw;
			p.y -= yw;
			break;
		case 2:
			p.x += xw;
			p.y -= yw;
			break;
		case 3:
			p.x += xw;
			p.y += yw;
			break;
		case 4:
			p.x -= xw;
			p.y += yw;
			break;
		}


		t = map->GetTile(layer, &p);

		if (t && TILE_MAP_RESOURCE_ID == t->GetObjectType())
		{
			UseResource((TileMapResource *)t);
		}
	}
}

void Entity::Refresh(hgeVector &new_pos)
{
	if (state_id == ENTITY_DEATH)
	{
		death->Stop();
		death->SetFrame(0);

		current_animation = NULL;

		state_id = ENTITY_NONE_STATE;
		life = 100;

		Stand();

		in_air = true;
		in_run = false;
	}

	pos = old_pos = new_pos;
}

void Entity::Kill()
{
	life = 0;
	Death();
}