#include <hgegui.h>
#include <hgeguictrls.h>
#include <hgeresource.h>
#include <stdio.h>
#include <io.h>
#include "MenuItem.h"
#include "Options.h"


extern hgeGUI *gui;
extern HGE *hge;
extern hgeResourceManager *resource_manager;

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

hgeQuad menu_bg;



void RenderGUI()
{
	hge->Gfx_RenderQuad(&menu_bg);
	gui->Render();
}

void UpdateBG()
{
	static float t = 0.0f;
	float dt=hge->Timer_GetDelta();
	float tx,ty;

	t += dt;
	tx = 50*cosf(t/60);
	ty = 50*sinf(t/60);

	menu_bg.v[0].tx=tx;					menu_bg.v[0].ty=ty;
	menu_bg.v[1].tx=tx+SCREEN_WIDTH/64;	menu_bg.v[1].ty=ty;
	menu_bg.v[2].tx=tx+SCREEN_WIDTH/64;	menu_bg.v[2].ty=ty+SCREEN_HEIGHT/64;
	menu_bg.v[3].tx=tx;					menu_bg.v[3].ty=ty+SCREEN_HEIGHT/64;

}

void InitiateBG()
{
	menu_bg.tex = resource_manager->GetTexture("menu_bg");

	menu_bg.blend = BLEND_ALPHABLEND|BLEND_COLORMUL|BLEND_NOZWRITE;

	for(int i=0;i<4;i++)
	{
		menu_bg.v[i].z = 0.5f;
		menu_bg.v[i].col = 0xFFFFFFFF;
	}

	menu_bg.v[0].x=0;					menu_bg.v[0].y=0; 
	menu_bg.v[1].x=SCREEN_WIDTH;		menu_bg.v[1].y=0; 
	menu_bg.v[2].x=SCREEN_WIDTH;		menu_bg.v[2].y=SCREEN_HEIGHT; 
	menu_bg.v[3].x=0;					menu_bg.v[3].y=SCREEN_HEIGHT; 
}

void InitiateGUI1()
{
	delete gui;
	gui = new hgeGUI();

	hgeFont *fnt = resource_manager->GetFont("font1");
	HEFFECT	snd = NULL;

	gui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, SCREEN_WIDTH/2, 200, 0.0f, "Play"));
	//gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, SCREEN_WIDTH/2, 240, 0.1f, "Connect"));
	gui->AddCtrl(new hgeGUIMenuItem(3, fnt, snd, SCREEN_WIDTH/2, 240, 0.2f, "Options"));
	gui->AddCtrl(new hgeGUIMenuItem(4, fnt, snd, SCREEN_WIDTH/2, 280, 0.3f, "About"));
	gui->AddCtrl(new hgeGUIMenuItem(5, fnt, snd, SCREEN_WIDTH/2, 320, 0.4f, "Quit"));

	gui->SetNavMode(HGEGUI_UPDOWN|HGEGUI_CYCLED);
	gui->SetFocus(1);
	gui->Enter();

	hgeSprite *cursor = resource_manager->GetSprite("cursor");
	
	InitiateBG();

	gui->SetCursor(cursor);
}

int UpdateGUI1()
{
	float dt=hge->Timer_GetDelta();

	int id;
	static int last_id = 0;

	int result = -1;
	
	UpdateBG();

	id = gui->Update(dt);
	if(id == -1)
	{
		gui->SetFocus(1);
		gui->Enter();

		result = last_id;
/*
		switch(last_id)
		{
			case 1:
				result = 1;
				break;
			case 2:
				result = 2;
				break;
			case 3:
				result = 3;
				break;
			case 4:
				result = 4;
				break;
			case 5: 
				result = 5;
				break;
		}
*/
	}
	else if(id) 
	{ 
		last_id = id;
		gui->Leave();
	}

	return result;
}



void InitiateGUI2()
{
	delete gui;
	gui = new hgeGUI();

	hgeFont *fnt = resource_manager->GetFont("font1");
	HEFFECT	snd = NULL;

	hgeGUIListbox *list_box = new hgeGUIListbox(1, 40, 40, 800, 600, fnt, 
		ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), ARGB(100, 255, 255, 0));

	_finddata_t find;
	char path_find[MAX_PATH];
	
	strcpy(path_find, PATH_T0_LEVELS);
	strcat(path_find, "\\*.lev");
	char info[MAX_PATH];

	int result = _findfirst(path_find, &find);

	int flag = result;
	while (flag != -1)
	{
		list_box->AddItem(find.name);
		flag = _findnext(result, &find);
	}
	_findclose(result);

	gui->AddCtrl(list_box);
	gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 700, 640, 0.0f, "Create"));
	gui->AddCtrl(new hgeGUIMenuItem(3, fnt, snd, 800, 640, 0.0f, "Cancel"));


	gui->SetNavMode(HGEGUI_UPDOWN|HGEGUI_CYCLED);
	gui->SetFocus(1);
	gui->Enter();

	hgeSprite *cursor = resource_manager->GetSprite("cursor");

	gui->SetCursor(cursor);
}

int UpdateGUI2()
{
	float dt=hge->Timer_GetDelta();

	int id;
	static int last_id = 0;

	int result = -1;
	
	UpdateBG();

	id = gui->Update(dt);
	if(id == -1)
	{
		if (last_id != 1)
		{
			gui->SetFocus(1);
			gui->Enter();
		}

		result = last_id;
	}
	else if(id) 
	{ 
		
		last_id = id;
		if (last_id != 1)
			gui->Leave();
	}

	return result;
}

void InitiateGUI3()
{
	delete gui;
	gui = new hgeGUI();

	hgeFont *fnt = resource_manager->GetFont("font1");
	HEFFECT	snd = NULL;

	hgeGUIListbox *list_box = new hgeGUIListbox(1, 40, 40, 800, 600, fnt, 
		ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), ARGB(100, 255, 255, 0));

	gui->AddCtrl(list_box);
	gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 600, 640, 0.0f, "Connect"));
	gui->AddCtrl(new hgeGUIMenuItem(3, fnt, snd, 700, 640, 0.0f, "Update"));
	gui->AddCtrl(new hgeGUIMenuItem(4, fnt, snd, 800, 640, 0.0f, "Cancel"));


	gui->SetNavMode(HGEGUI_UPDOWN|HGEGUI_CYCLED);
	gui->SetFocus(1);
	gui->Enter();

	hgeSprite *cursor = resource_manager->GetSprite("cursor");

	gui->SetCursor(cursor);
}

int UpdateGUI3()
{
	float dt=hge->Timer_GetDelta();

	int id;
	static int last_id = 0;

	int result = -1;
	
	UpdateBG();

	id = gui->Update(dt);
	if(id == -1)
	{
		if (last_id != 1)
		{
			gui->SetFocus(1);
			gui->Enter();
		}

		result = last_id;
	}
	else if(id) 
	{ 
		
		last_id = id;
		if (last_id != 1)
			gui->Leave();
	}
	
	return result;
}


void InitiateGUI4()
{
	delete gui;
	gui = new hgeGUI();

	hgeFont *fnt = resource_manager->GetFont("font1");
	HEFFECT	snd = NULL;

	
	gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, SCREEN_WIDTH-100, SCREEN_HEIGHT-100, 0.0f, "Cancel"));

	gui->SetNavMode(HGEGUI_UPDOWN|HGEGUI_CYCLED);
	//gui->SetFocus(1);
	gui->Enter();

	hgeSprite *cursor = resource_manager->GetSprite("cursor");

	gui->SetCursor(cursor);
}

int UpdateGUI4()
{
	float dt=hge->Timer_GetDelta();

	int id;
	static int last_id = 0;

	int result = -1;
	
	UpdateBG();

	id = gui->Update(dt);
	if(id == -1)
	{
		if (last_id != 1)
		{
			gui->SetFocus(1);
			gui->Enter();
		}

		result = last_id;
	}
	else if(id) 
	{ 
		
		last_id = id;
		if (last_id != 1)
			gui->Leave();
	}

	return result;
}

void InitiateGUIOptions()
{
	delete gui;
	gui = new hgeGUI();

	hgeFont *fnt = resource_manager->GetFont("font1");
	HEFFECT	snd = NULL;

	hgeGUIListbox *list_box = new hgeGUIListbox(1, 40, 40, 600, 200, fnt, 
		ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), ARGB(100, 255, 255, 0));

	list_box->AddItem("800x600");
	list_box->AddItem("1024x768");


	gui->AddCtrl(list_box);

	list_box = new hgeGUIListbox(4, 40, 240, 600, 200, fnt, 
		ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), ARGB(100, 255, 255, 0));

	list_box->AddItem("FULL SCREEN");
	list_box->AddItem("WINDOWED");

	gui->AddCtrl(list_box);

	gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, SCREEN_WIDTH-200, SCREEN_HEIGHT-100, 0.0f, "OK"));
	gui->AddCtrl(new hgeGUIMenuItem(3, fnt, snd, SCREEN_WIDTH-100, SCREEN_HEIGHT-100, 0.0f, "Cancel"));


	gui->SetNavMode(HGEGUI_UPDOWN|HGEGUI_CYCLED);
	gui->SetFocus(1);
	gui->Enter();

	hgeSprite *cursor = resource_manager->GetSprite("cursor");

	gui->SetCursor(cursor);
}

int UpdateGUIOptions()
{
	float dt=hge->Timer_GetDelta();

	int id;
	static int last_id = 0;

	int result = -1;
	
	UpdateBG();

	id = gui->Update(dt);
	if(id == -1)
	{
		if (last_id != 1)
		{
			gui->SetFocus(1);
			gui->Enter();
		}

		result = last_id;
	}
	else if(id) 
	{ 
		
		last_id = id;
		if (last_id != 1)
			gui->Leave();
	}

	return result;
}