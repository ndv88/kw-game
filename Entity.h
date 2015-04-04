#pragma once

#include "AABB.h"

enum ENTYTY_STATE
{
	ENTITY_NONE_STATE = -1,
	ENTITY_STANDING = 0,
	ENTITY_RUNNING = 1,
	ENTITY_SKIDDING = 2,
	ENTITY_JUMPING = 3,
	ENTITY_FALLING = 4,
	ENTITY_DEATH = 5
};

struct EntityInfo
{
	int life;
	int armour;
	int weapon_id;

	short xw, yw;

	float friction_run;
	float friction_skid;
	float jump_force;
	float jump_bias;

	bool face_direction;

	char *stand;
	char *run;
	char *jump;
	char *fall;
	char *skid;
	char *death;
};

struct ENTITY_NET_DATA
{
	int id;
	hgeVector pos;
	hgeVector old_pos;

	short state_id;
	int skin_id;
	int weapon_id;

	bool in_run;
	bool in_air;
	bool face_direction;

	int life;
	int armour;

	float world_offset_x;
	float world_offset_y;

	bool is_disconnect;

	hgeVector target;
	bool attack;
};

struct ENTITY_NET_DATA_SMALL
{
	int id;
	hgeVector pos;
	hgeVector old_pos;

	short state_id;

	bool in_run;
	bool in_air;
	bool face_direction;

	float world_offset_x;
	float world_offset_y;

	bool is_disconnect;

	hgeVector target;
	bool attack;
};

class Weapon;
class Bullet;

class TileMapResource;

class Entity: public AABB
{
	Entity();
public:
	Entity::Entity(hgeVector pos, short layer);
	~Entity();

	void SetFaceDirection(int direction) { this->face_direction = direction; }

	virtual void Verlet();

	void SetId(int id) { this->id = id; }
	int GetId() { return id; }

	void Initiate(int skin_id);
	void SetWeapon(int weapon_id);

	virtual void ReportCollision(float px, float py, float dx, float dy, TileMap *t);

	virtual void Move(int move);

	virtual void Render();

	virtual int Update();

	int UpdateIsNetworkGame();

	virtual void UpdateGraphic();
	
	void Think();

	void Attack(hgeVector &target);
	void RotateWeapon(hgeVector &target);
	void KillEntity(Bullet *bullet);
	
	void FaceToTarget(hgeVector &v);

	void Stand();
	void Run();
	void Jump();
	void Fall();
	void Skid();
	void Death();


	virtual void UseResource(TileMapResource *t);

	void GetEntityData(ENTITY_NET_DATA &data);
	void SetEntityData(ENTITY_NET_DATA &data);

	void GetEntityData(ENTITY_NET_DATA_SMALL &data);
	void SetEntityData(ENTITY_NET_DATA_SMALL &data);

	virtual inline void Reset();
	virtual bool IsDisconnect();

	int GetLife()	{ return life; }
	int GetState()	{ return state_id; }

	void Refresh(hgeVector &new_pos);

	void Kill();

protected:
	void EndStand();
	void EndRun();
	void EndJump();
	void EndFall();
	void EndSkid();
	void ExitState();
	

	virtual int Resolve(float x, float y, TileMap *t);

	void TestVsResource();

	
public:
	hgeVector target;
	bool attack_flag;

	bool is_disconnect;

protected:

	float timer;

	int id;

	int skin_id;
	int weapon_id;

	short state_id;
	bool in_run;
	bool in_air;

	bool face_direction;

	int life;
	int armour;
	Weapon *weapon;

	float friction_run;
	float friction_skid;
	float jump_force;
	float jump_bias;

	hgeAnimation *stand;
	hgeAnimation *run;
	hgeAnimation *jump;
	hgeAnimation *fall;
	hgeAnimation *skid;
	hgeAnimation *death;
	hgeAnimation *current_animation;
};

