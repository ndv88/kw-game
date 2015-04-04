#include <hge.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <hgeresource.h>
#include <time.h>
#include <fstream>

#include "Options.h"

#include "Initiate.h"

#include "Map.h"
#include "World.h"
#include "Ray.h"

#include "Entity.h"
#include "DefinitionGameObjects.h"

#include "GUI.h"

//#include "Network.h"
//#include "Server.h"
//#include "Client.h"

#pragma comment(lib, "hge.lib")
#pragma comment(lib, "hgehelp.lib")


int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool WINDOWED = true;

HGE	*hge = NULL;
hgeGUI *gui = NULL;
hgeFont *text = NULL;


std::string level_name;
std::string next_level_name;

hgeResourceManager *resource_manager = NULL;

HANDLE wait_clients = NULL;
HANDLE srv_accept = NULL;
HANDLE find_srv = NULL;
HANDLE client_thread = NULL;


int menu = 1;

hgeVector mouse_pos;
hgeSprite *mouse_cursor;
hgeAnimation *hard;

Entity *player;
World *world;

int level_end = 0;

bool FrameFunc();
bool RenderFunc();

bool StartNewGame();
bool CreateNewGame();
bool ConnectToGame();
void CloseGame();


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_TITLE, "KW");

	int w = 0;
	int h = 0;
	bool mode = false;

	std::ifstream f("options.cfg", std::ios::binary);
	
	if (f.is_open())
	{
		f.read((char *)&w, sizeof(int));
		f.read((char *)&h, sizeof(int));
		f.read((char *)&mode, sizeof(bool));
		f.close();
	}

	if (w != 0 && h != 0)
	{
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;
		WINDOWED = mode;
	}
			
	

	hge->System_SetState(HGE_WINDOWED, WINDOWED);
	hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_LOGFILE, "debug.log");
	hge->System_SetState(HGE_DONTSUSPEND, true);

	//hge->System_SetState(HGE_FPS, 40);

	if(hge->System_Initiate())
	{
		resource_manager = CreateResourceManager();
		resource_manager->ChangeScript("resource.rs");

		mouse_cursor = resource_manager->GetSprite("target");
		text = resource_manager->GetFont("font2");
		hard = new hgeAnimation( *(resource_manager->GetAnimation( "hard" )) );
		hard->Play();

		InitiateGUI1();

		world = InitiateWorld();

		hge->System_Start();
	}

	delete hard;
	delete world;

	delete mouse_cursor;

	delete gui;
	
	hge->System_Shutdown();
	hge->Release();

	HANDLE mutex;

	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX1);
	CloseHandle(mutex);

	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	CloseHandle(mutex);

	return 0;
}

bool FrameFunc()
{
	if (menu == 1)
	{
		int id = UpdateGUI1();

		switch(id)
		{
		case 1:
			//menu = 2;
			//InitiateGUI2();

			level_name = "demo.lev";
			//if (StartNewGame())
			{
				menu = 0;
				CreateNewGame();
				
			}

			break;
		case 2:
			{
			//menu = 2;
			//InitiateGUI2();

			//HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX1);
			//if (!mutex)
			//	mutex = CreateMutex(NULL, FALSE, NAME_MUTEX1);

			//hgeGUIObject *lb = gui->GetCtrl(1);
			//find_srv = CreateThread(NULL, 0, &FindServer, lb, 0, NULL);

			}
			break;
		case 3:
			menu = 2;
			InitiateGUIOptions();
			break;
		case 4:
			menu = 4;
			InitiateGUI4();
			break;
		case 5:
			return true;
			break;
		}
	}
	else if (menu == 2)
	{
		int id = UpdateGUI2();
		switch(id)
		{
		case 2:
			/*if (StartNewGame())
			{
				CreateNewGame();
				menu = 0;
			}*/
			{
			hgeGUIListbox *lb = (hgeGUIListbox *)gui->GetCtrl(1);
			int item = lb->GetSelectedItem();
			char *buffer = lb->GetItemText(item);

			lb = (hgeGUIListbox *)gui->GetCtrl(4);
			item = lb->GetSelectedItem();
			char *temp = lb->GetItemText(item);

			bool screen_mode = false;
			if (strcmp("FULL SCREEN", temp) == 0)
			{
				screen_mode = false;
			}
			else if (strcmp("WINDOWED", temp) == 0)
			{
				screen_mode = true;
			}

			std::string str(buffer);
			int index = str.find("x", 0);
			std::string sub_str1 = str.substr(0, index);
			std::string sub_str2 = str.substr(index+1);

			int w = atoi(sub_str1.c_str());
			int h = atoi(sub_str2.c_str());

			std::ofstream f("options.cfg", std::ios::binary);
			
			f.write((const char *)&w, sizeof(int));
			f.write((const char *)&h, sizeof(int));
			f.write((const char *)&screen_mode, sizeof(bool));
			
			f.close();

			menu = 1;
			InitiateGUI1();
			}
			

			break;
		case 3:
			menu = 1;
			InitiateGUI1();
			break;

		}
	}
	else if (menu == 3)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX1);
		if (mutex)
		{
			WaitForSingleObject(mutex, INFINITE);

			int id = UpdateGUI3();
			switch(id)
			{
			case 2:
				/*TerminateThread(find_srv, 0);
				if (ConnectToGame())
				{
					menu = 0;
				}*/
				break;
			case 3:
				break;
			case 4:
				menu = 1;
				//TerminateThread(find_srv, 0);
				//find_srv = NULL;
				InitiateGUI1();
				break;
			}
			ReleaseMutex(mutex);
		}
	}
	else if (menu == 4)
	{
		int id = UpdateGUI4();
		switch(id)
		{
		case 2:
			menu = 1;
			InitiateGUI1();
			break;

		}
	}
	else
	{
		static float time = 0;

		hge->Input_GetMousePos(&mouse_pos.x, &mouse_pos.y);
		float dt = hge->Timer_GetDelta();
		time += dt;

		hard->Update(dt);

		if (time > 0.025000)//40 fps
		{
			if (hge->Input_GetKeyState(HGEK_ESCAPE))
			{
				menu = 1;

				CloseGame();
				InitiateGUI1();
				return false;
			}	

			world->Reset();

		

			if (ENTITY_DEATH != player->GetState())
			{
				HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
				if (mutex)
				{
					WaitForSingleObject(mutex, INFINITE);

					if (hge->Input_GetKeyState(HGEK_SPACE) || hge->Input_GetKeyState(HGEK_UP))
					{
						player->Jump();
					}
					if (hge->Input_GetKeyState(HGEK_A) || hge->Input_GetKeyState(HGEK_LEFT))
					{
						player->Move(MOVE_LEFT);
					}
					if (hge->Input_GetKeyState(HGEK_D) || hge->Input_GetKeyState(HGEK_RIGHT))
					{
						player->Move(MOVE_RIGHT);
					}
					if (hge->Input_GetKeyState(HGEK_K))
					{
						player->Kill();
					}

					player->FaceToTarget(mouse_pos);


					if (world->GetWorldEntitys()->size() == 0)
					{
						if (!next_level_name.empty())
						{
							level_end = 1;
						}
						else
						{
							level_end = 2;
						}

						static float tmp = 0;
						tmp += hge->Timer_GetDelta();
						if (tmp > 0.40)
						{
							if (hge->Input_GetKeyState(HGEK_LBUTTON))
							{
								level_end = 0;

								level_name = next_level_name;

								if (!level_name.empty())
								{
									CreateNewGame();
								}
								else
								{
									menu = 1;

									CloseGame();
									InitiateGUI1();
									return false;
								}
								tmp = 0;
							}
						}
					}


					if (hge->Input_GetKeyState(HGEK_LBUTTON))
					{
						player->Attack(mouse_pos);
					}
					else
					{
						player->RotateWeapon(mouse_pos);
					}

					world->GetMap()->ScrollTo(player->GetPos());

					ReleaseMutex(mutex);
				}				
			}
			else
			{
				if (hge->Input_GetKeyState(HGEK_SPACE))
				{
					UpdatePlayerPos();

					HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
					if (mutex)
					{
						WaitForSingleObject(mutex, INFINITE);

						world->GetMap()->ScrollTo(player->GetPos());

						ReleaseMutex(mutex);
					}
				}
			}



			world->UpdatePositionObjects();
			world->UpdateObjects();
	 
			time = 0;
		}

		if (world->GetWorldEntitys()->size() == 0)
		{
			if (!next_level_name.empty())
			{
				level_end = 1;
			}
			else
			{
				level_end = 2;
			}
		}

		world->UpdateGraphic();
	}

	return false;
}

bool RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	if (menu)
	{
		HANDLE mutex1 = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX1);
		if (mutex1)
			WaitForSingleObject(mutex1, INFINITE);


		RenderGUI();
		ReleaseMutex(mutex1);

		if (menu == 4)
		{
			text->printf(SCREEN_WIDTH/2, 200, HGETEXT_CENTER, "NESTERENKO DIMA");
			text->printf(SCREEN_WIDTH/2, 250, HGETEXT_CENTER, "COMPUTER ACADEMY \"STEP\"");
		}
	}
	else
	{
		world->Render();
		mouse_cursor->Render(mouse_pos.x, mouse_pos.y);

		if (text)
		{
			char str[8];
			int life;

			HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
			if (mutex)
			WaitForSingleObject(mutex, INFINITE);
			life = player->GetLife();
			ReleaseMutex(mutex);

			hard->Render(32, 32);
			text->printf(74, 16, HGETEXT_LEFT, "%d", life);

			if (level_end == 1)
			{
				text->printf(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, HGETEXT_CENTER, "GO TO NEXT LEVEL!!!");
			}
			else if (level_end == 2)
			{
				text->printf(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, HGETEXT_CENTER, "YOU WIN!!!");
			}

			if (ENTITY_DEATH == player->GetState())
			{
				text->printf(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, HGETEXT_CENTER, "Please press a space.....");
			}
		}
	}

	hge->Gfx_EndScene();
	return false;
}

bool StartNewGame()
{
	hgeGUIListbox *lb = (hgeGUIListbox *)gui->GetCtrl(1);
	if (0 < lb->GetNumItems())
	{

		int n = lb->GetSelectedItem();
		level_name = lb->GetItemText(n);
		return true;
	}
	return false;
}

bool CreateNewGame()
{

	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	if (!mutex)
		mutex = CreateMutex(NULL, FALSE, NAME_MUTEX2);

	Map *map = new Map(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 64, 64);
	char path[MAX_PATH];

	strcpy(path, PATH_T0_LEVELS);
	strcat(path, "\\");
	strcat(path, level_name.c_str());

	int count_entitys = 0;

	map->Load(path, count_entitys, next_level_name);
	map->UpdateEdges(GENERAL_MAP_LAYER);
	
	world->Create(map, map->GetLayers());

	Ray::tiles = map;

	InitiatePlayer(0);
	//RefreshPlayer(player);

	

		
	srand((unsigned)time(NULL));
	for (int i = 0; i < count_entitys; i++)
	{
		Entity *e = new Entity(hgeVector(128, 128), GENERAL_MAP_LAYER);
		
		int skin_id = rand()%2+1;

		e->Initiate(skin_id);
		e->SetSpeed(3.5f, 4.5f);

		Sleep(40);
		RefreshPlayer(e);

		world->InsertObject(e);
	}

	//wait_clients = CreateThread(NULL, 0, &WaitClients, NULL, 0, NULL);
	//srv_accept = CreateThread(NULL, 0, &Accept, (void *)world->GetWorldEntitys(), 0, NULL);

	return true;
	

}

bool ConnectToGame()
{
	hgeGUIListbox *lb = (hgeGUIListbox *)gui->GetCtrl(1);
	
	if (0 < lb->GetNumItems())
	{
		int n = lb->GetSelectedItem();
		int find = 0;

		std::string str, srv_ip;
		str = lb->GetItemText(n);

		find = str.find_first_of("|");
		srv_ip = str.substr(0, find);
		level_name = str.substr(find+1);

		Map *map = new Map(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 64, 64);
		char path[MAX_PATH];
		
		strcpy(path, PATH_T0_LEVELS);
		strcat(path, "\\");
		strcat(path, level_name.c_str());

		//map->Load(path);
		//map->UpdateEdges(GENERAL_MAP_LAYER);
		
		world->Create(map, map->GetLayers());

		Ray::tiles = map;

		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
		if (!mutex)
			mutex = CreateMutex(NULL, FALSE, NAME_MUTEX2);

		//SOCKET sock = ConnectToServer((void *)world, srv_ip);
		//if (sock != NULL)
		//{
		//	NET_DATA *nd = new NET_DATA;
		//	nd->sock = sock;
		//	nd->data = (void *)world->GetWorldEntitys();

		//	client_thread = CreateThread(NULL, 0, &Client, (void *)nd, 0, NULL);
		//}

		return true;
	}

	return false;
}

void CloseGame()
{
	TerminateThread(wait_clients, 0);
	CloseHandle(wait_clients);
	TerminateThread(srv_accept, 0);
	CloseHandle(srv_accept);
	wait_clients = NULL;
	srv_accept = NULL;


	TerminateThread(client_thread, 0);
	CloseHandle(client_thread);
	client_thread = NULL;

	world->Free();

}

