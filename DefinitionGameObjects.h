#pragma once

#include "Entity.h"
#include "Weapon.h"
#include "Bullet.h"


BulletInfo bullets[] =
{
	{50, 2, 2, 2.1, 3.1, 22, 32, "bullet1", "particle_trail1", "particle_detonation1"},
	{10, 2, 2, 1.1, 2.1, 22, 32, "bullet1", "particle_trail2", "particle_detonation1"},
	{10, 2, 2, 1.1, 2.1, 22, 32, "bullet1", "particle_trail3", "particle_detonation2"}
};

WeaponInfo weapons[] = 
{
	{0, 50, 8, 0.05, "", "weapon1"},
	{1, 50, 8, 0.30, "", "weapon1"},
	{2, 50, 8, 0.30, "", "weapon1"}
};

EntityInfo entitys[] = 
{
	{100, 0, 0, 32, 32, 0.01, 0.1, 8, 1.5, false, "stand1", "run1", "jump1", "fall1", "skid1", "death1"},
	{100, 0, 1, 32, 32, 0.01, 0.1, 8, 1.5, false, "stand2", "run2", "jump2", "fall2", "skid2", "death2"},
	{100, 0, 1, 32, 32, 0.01, 0.1, 8, 1.5, false, "stand3", "run3", "jump3", "fall3", "skid3", "death3"},
	{100, 0, 2, 32, 32, 0.01, 0.1, 8, 1.5, false, "stand4", "run4", "jump4", "fall4", "skid4", "death4"}
};