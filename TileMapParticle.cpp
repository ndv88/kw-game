#include "TileMapParticle.h"

TileMapParticle::TileMapParticle()
: TileMap()
, ps(NULL)
{
	TYPE_ID = TILE_MAP_PARTICLE_ID;

	ps_name = "";
	ps = NULL;
}


TileMapParticle::TileMapParticle(char *texture_name, char *name_particle, hgeVector pos, short xw, short yw, short layer)
: TileMap(texture_name, pos, xw, yw, layer)
{
	TYPE_ID = TILE_MAP_PARTICLE_ID;

	ps_name = name_particle;
	
	ps = new hgeParticleSystem( *(resources_manager->GetParticleSystem( ps_name.c_str() )) );

	ps->FireAt(pos.x, pos.y);
}

TileMapParticle::~TileMapParticle()
{
	if (ps)
	{
		delete ps;
		ps = NULL;
	}

	ps_name.clear();
}


void TileMapParticle::Render()
{
	TileMap::Render();

	if (ps)
	{
		ps->Render();
	}
}

void TileMapParticle::Save(std::ofstream *f)
{
	TileMap::Save(f);

	size_t l = ps_name.length();

	f->write((const char *)&l, sizeof(size_t));
	f->write(ps_name.c_str(), l*sizeof(char));

}

void TileMapParticle::Load(std::ifstream *f)
{
	TileMap::Load(f);
	ps_name.clear();

	size_t l = 0;
	f->read((char *)&l, sizeof(size_t));

	char *name = new char[l];
	f->read(name, l*sizeof(char));

	ps_name = name;

	if (ps)
	{
		delete ps;
	}

	ps = new hgeParticleSystem( *(resources_manager->GetParticleSystem( ps_name.c_str() )) );
	ps->Fire();
}

void TileMapParticle::UpdateGraphic()
{
	float dt = hge->Timer_GetDelta();
	ps->Update(dt);
}

void TileMapParticle::Play()
{
	ps->Fire();
}

void TileMapParticle::Stop()
{
	ps->Stop();
}

void TileMapParticle::MoveParticle(float x, float y, bool move)
{
	ps->MoveTo(x, y, move);
}