#include <time.h>

#include "Initiate.h"
#include <hgeresource.h>

#include "GraphicObject.h"
#include "GraphicDynamicObject.h"

#include "World.h"
#include "Background.h"

extern Entity *player;
extern World *world;

hgeResourceManager* CreateResourceManager()
{
	hgeResourceManager *rm = new hgeResourceManager;
	GraphicObject::SetResourceManager(rm);
	Background::resources_manager = rm;
	return rm;
}

World* InitiateWorld()
{
	World *world = new World;
	GraphicObject::SetWorld(world);
	return world;
}

void InitiatePlayer(int id)
{
	player = new Entity(hgeVector(350, 400), 3);
	player->Initiate(0); 
	//player->SetWeapon(0);
	player->SetSpeed(6.5f, 8.5f);
	player->SetId(id);
	world->SetPlayer(player);

	UpdatePlayerPos();
}

void UpdatePlayerPos()
{
	hgeVector p(350, 400);

	Map *map = world->GetMap();
	map->SetOffsetX(0);
	map->SetOffsetY(0);

	world->UpdatePositionObjects();

	int rows = map->GetRows();
	int cols = map->GetCols();

	bool work = true;

	while (work)
	{
		work = true;
		

		int row, col;
		map->GetTileIndex(&p, row, col);

		TileMap *prev_tile = NULL;
		TileMap *tile = NULL;

		for (int i = row; i < (rows-1); i++)
		{
			tile = map->GetTile(GENERAL_MAP_LAYER, i+1, col);
			prev_tile = map->GetTile(GENERAL_MAP_LAYER, i, col);

			if (tile->GetTileType() == TILE_TYPE_FULL && prev_tile->GetTileType() == TILE_TYPE_EMPTY)
			{
				player->Refresh(prev_tile->GetPos());

				work = false;
				break;
			}
		}
	}
}

void RefreshPlayer(Entity *guy)
{
	Map *map = world->GetMap();
	map->SetOffsetX(0);
	map->SetOffsetY(0);
	world->SetWorldOffsetX(0);
	world->SetWorldOffsetY(0);

	int rows = map->GetRows();
	int cols = map->GetCols();

	bool work = true;

	while (work)
	{
		work = true;
		
		Sleep(10);
		int col = rand()%(cols-2)+1;

		TileMap *prev_tile = NULL;
		TileMap *tile = NULL;

		for (int i = 1; i < (rows-1); i++)
		{
			tile = map->GetTile(GENERAL_MAP_LAYER, i+1, col);
			prev_tile = map->GetTile(GENERAL_MAP_LAYER, i, col);

			if (tile->GetTileType() == TILE_TYPE_FULL && prev_tile->GetTileType() == TILE_TYPE_EMPTY)
			{
				guy->Refresh(prev_tile->GetPos());

				work = false;
				break;
			}
		}
	}
/**/

}