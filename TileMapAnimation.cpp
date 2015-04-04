#include "TileMapAnimation.h"

TileMapAnimation::TileMapAnimation()
: TileMap()
, animation(NULL)
{
	TYPE_ID = TILE_MAP_ANIMATION_ID;
}

TileMapAnimation::TileMapAnimation(char *res_name, hgeVector pos, short xw, short yw, short layer)
: TileMap(res_name, pos, xw, yw, layer)
{
	TYPE_ID = TILE_MAP_ANIMATION_ID;

	if (res_name != NULL)
	{
		animation = new hgeAnimation( *(resources_manager->GetAnimation(resource_name.c_str())) );

		animation->Play();
	}
}

TileMapAnimation::~TileMapAnimation()
{
	if (animation)
	{
		delete animation;
		animation = NULL; 
	}
}

void TileMapAnimation::Render()
{
	float x0, y0, x1, y1, x2, y2, x3, y3;

	x0 = pos.x - xw; y0 = pos.y - yw;
	x1 = pos.x + xw; y1 = pos.y - yw;
	x2 = pos.x + xw; y2 = pos.y + yw;
	x3 = pos.x - xw; y3 = pos.y + yw;

	if (animation)
	{
		animation->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
	}
}

void TileMapAnimation::Save(std::ofstream *file)
{
	TileMap::Save(file);
}

void TileMapAnimation::Load(std::ifstream *file)
{
	TileMap::Load(file);

	if (animation != NULL)
		delete animation;

	animation = new hgeAnimation( *(resources_manager->GetAnimation( resource_name.c_str())) );
	animation->Play();
}

void TileMapAnimation::UpdateGraphic()
{
	float dt = hge->Timer_GetDelta();
	animation->Update(dt);
}

void TileMapAnimation::Stop()
{
	animation->Stop();
}

void TileMapAnimation::Play()
{
	animation->Play();
}