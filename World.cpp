#include "World.h"
#include "Bullet.h"

extern HGE *hge;

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

World::World()
: map(NULL)
, layers(0)
, map_offset_x(0) ,map_offset_y(0) 
{
	TYPE_ID = WORLD_ID;

	player = NULL;
	bg = new Background(SCREEN_WIDTH, SCREEN_HEIGHT);
	bg->Initiate();
}

World::~World()
{
	if (bg)
		delete bg;

	Free();
}

void World::Create(Map *map, size_t world_layers)
{
	Free();

	this->map = map;
	map_offset_x = map->GetOffsetX();
	map_offset_y = map->GetOffsetY();

	layers = world_layers;

	entitys.resize(0);
	bullets.resize(0);

	//for (size_t l = 0; l < layers; l++)
	//{
	//	entitys[l].resize(0);
	//	bullets[l].resize(0);
	//}
}



void World::Free()
{
	//for (size_t l = 0; l < layers; l++)
	//{
		size_t i;
		for (i = 0; i < bullets.size(); i++)
		{
			delete bullets[i];
			bullets[i] = NULL;

		}
		bullets.clear();


		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
		if (mutex)
		{
			WaitForSingleObject(mutex, INFINITE);

			if (player)
			{
				delete player;
				player = NULL;
			}

			for (i = 0; i < entitys.size(); i++)
			{
				delete entitys[i];
				entitys[i] = NULL;
			}
			entitys.clear();

			ReleaseMutex(mutex);
		}
	//}

	layers = 0;

	if (map)
	{
		delete map;
		map = NULL;
	}


	map_offset_x = map_offset_y = 0;


}

void World::InsertObject(GraphicObject *obj)
{
	short type = obj->GetObjectType();

	size_t index = 0;

	bool flag = true;

	if (type == BULLET_ID)
	{
		for (index = 0; index < bullets.size(); index++)
		{
			if (obj == bullets[index])
				return;
		}
		bullets.push_back((Bullet *)obj);
	}
	else if (type == ENTITY_ID)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
		if (mutex)
		{
			WaitForSingleObject(mutex, INFINITE);

			for (index = 0; index < entitys.size(); index++)
			{
				if (obj == entitys[index])
					return;
			}
			entitys.push_back((Entity *)obj);

			ReleaseMutex(mutex);
		}
	}

	//HGE *hge = hgeCreate(HGE_VERSION);
	//hge->System_Log("World::InsertObject");
	//hge->Release();

	
}

void World::RemoveObject(GraphicObject *obj)
{
	short type = obj->GetObjectType();

	if (type == BULLET_ID)
	{
		std::vector<Bullet *>::iterator it = bullets.begin();

		while (it != bullets.end())
		{
			if (*it == obj)
			{
				delete (*it);
				bullets.erase(it);
				break;
			}
		}
	}
	else if (type == ENTITY_ID)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
		if (mutex)
		{
			WaitForSingleObject(mutex, INFINITE);

			std::vector<Entity *>::iterator it = entitys.begin();

			while (it != entitys.end())
			{
				if (*it == obj)
				{
					delete (*it);
					entitys.erase(it);
					break;
				}
			}

			ReleaseMutex(mutex);
		}
	}
}

void World::UpdateObjects()
{


	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	if (mutex)
	{
		WaitForSingleObject(mutex, INFINITE);
		
		if (player)
			player->Update();

		ReleaseMutex(mutex);
	}

	std::vector<Entity *>::iterator ie = entitys.begin();		

	while (ie != entitys.end())
	{
		if (ENTYTY_STATE::ENTITY_DEATH != (*ie)->Update())
		{
			(*ie)->Think();
			ie++;
		}
		else
		{
			delete (*ie);
			ie = entitys.erase(ie);
		}
	}


	std::vector<Bullet *>::iterator ib = bullets.begin();		

	while (ib != bullets.end())
	{
		if (BulletState::NONE != (*ib)->Update())
		{
				ib++;
		}
		else
		{
			delete (*ib);
			ib = bullets.erase(ib);
		}
	}
}

void World::UpdatePositionObjects()
{
	float current_map_offset_x = map->GetOffsetX();
	float current_map_offset_y = map->GetOffsetY();

	if ((map_offset_x != current_map_offset_x)
		|| (map_offset_y != current_map_offset_y))
	{
		float offset_x = map_offset_x - current_map_offset_x;
		float offset_y = map_offset_y - current_map_offset_y;

		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
		if (mutex)
		{
			WaitForSingleObject(mutex, INFINITE);

			for (size_t i = 0; i < entitys.size(); i++)
			{
				if (entitys[i] && !entitys[i]->is_disconnect)
				{
					entitys[i]->pos.x += offset_x;
					entitys[i]->pos.y += offset_y;
					entitys[i]->old_pos.x += offset_x;
					entitys[i]->old_pos.y += offset_y;
				}
			}

			if (player)
			{
				player->pos.x += offset_x;
				player->pos.y += offset_y;
				player->old_pos.x += offset_x;
				player->old_pos.y += offset_y;
			}


			ReleaseMutex(mutex);
		}

		for (size_t i = 0; i < bullets.size(); i++)
		{
			bullets[i]->pos.x += offset_x;
			bullets[i]->pos.y += offset_y;

		}


		map_offset_x = current_map_offset_x;
		map_offset_y = current_map_offset_y;
	}
}

void World::UpdateGraphic()
{
	bg->Update();
	map->UpdateGraphic();

	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	if (mutex)
	{
		WaitForSingleObject(mutex, INFINITE);

		for (int i = 0; i < entitys.size(); i++)
		{
			entitys[i]->UpdateGraphic();
		}

		if (player)
			player->UpdateGraphic();

		ReleaseMutex(mutex);

		for (size_t i = 0; i < bullets.size(); i++)
		{
			bullets[i]->UpdateGraphic();
		}	
	}


}

void World::Render()
{
	size_t i = 0;

	if (bg)
		bg->Render();

	for (size_t l = 0; l < layers; l++)
	{
		if (l == GENERAL_MAP_LAYER)
		{
			HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
			if (mutex)
			{
				WaitForSingleObject(mutex, INFINITE);

				for (int i = 0; i < entitys.size(); i++)
				{
					entitys[i]->Render();	
				}
			

				if (player)
				{
					if (l == player->GetLayer())
					{
						player->Render();
					}
				}

				ReleaseMutex(mutex);
			}
		}

		map->Render(l);

		if (l == GENERAL_MAP_LAYER)
		{
			for (i = 0; i < bullets.size(); i++)
			{
				bullets[i]->Render();
			}
		}
	}
}

void World::Reset()
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	if (mutex)
	{
		WaitForSingleObject(mutex, INFINITE);

		if (player)
		{
			player->Reset();
		}

		for (int i = 0; i < entitys.size(); i++)
		{
			if (!entitys[i]->is_disconnect)
				entitys[i]->Reset();
		}

		ReleaseMutex(mutex);
	}
}