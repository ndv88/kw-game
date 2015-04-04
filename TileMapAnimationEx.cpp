#include "TileMapAnimationEx.h"

TileMapAnimationEx::TileMapAnimationEx()
: TileMapAnimation()
{
	Object::TYPE_ID = TILE_MAP_ANIMATION_EX_ID;
	ps_name = "";
	ps = NULL;
}

TileMapAnimationEx::TileMapAnimationEx(char *animation_name, char *particle_name, hgeVector pos, short xw, short yw, short layer)
: TileMapAnimation(animation_name, pos, xw, yw, layer)
{
	Object::TYPE_ID = TILE_MAP_ANIMATION_EX_ID;

	if (particle_name != NULL)
	{
		ps_name = particle_name;
	
		ps = new hgeParticleSystem( *(resources_manager->GetParticleSystem( ps_name.c_str() )) );

		ps->FireAt(pos.x, pos.y);
	}
	else
	{
		ps = NULL;
	}
}

TileMapAnimationEx::~TileMapAnimationEx()
{
	if (ps)
	{
		delete ps;
		ps = NULL;
	}

	ps_name.clear();
}

void TileMapAnimationEx::Play()
{
	TileMapAnimation::Play();
	if (ps)
		ps->Fire();
	
}

void TileMapAnimationEx::Stop()
{
	TileMapAnimation::Stop();
	if (ps)
		ps->Stop();
}

void TileMapAnimationEx::Render()
{
	TileMapAnimation::Render();

	if (ps)
	{
		ps->Render();
	}
}

void TileMapAnimationEx::Save(std::ofstream *f)
{
	TileMapAnimation::Save(f);

	size_t l = ps_name.length();

	f->write((const char *)&l, sizeof(size_t));
	f->write(ps_name.c_str(), l*sizeof(char));
}

void TileMapAnimationEx::Load(std::ifstream *f)
{
	TileMapAnimation::Load(f);

	ps_name.clear();

	size_t l = 0;
	f->read((char *)&l, sizeof(size_t));

	char *name = new char[l];
	f->read(name, l*sizeof(char));
	name[l] = '\0';

	ps_name = name;

	
	if (ps)
	{
		delete ps;
		ps = NULL;
	}

	if (!ps_name.empty())
	{
		ps = new hgeParticleSystem( *(resources_manager->GetParticleSystem( ps_name.c_str() )) );
		ps->Fire();
	}
	

}

void TileMapAnimationEx::MoveParticle(float x, float y, bool move)
{
	if (ps)
		ps->MoveTo(x, y, move);
}

void TileMapAnimationEx::UpdateGraphic()
{
	TileMapAnimation::UpdateGraphic();

	float dt = hge->Timer_GetDelta();
	if (ps)
		ps->Update(dt);
}