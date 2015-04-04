#include "TileMapResource.h"



TileMapResource::TileMapResource()
: TileMapAnimationEx()
{
	Object::TYPE_ID = TILE_MAP_RESOURCE_ID;

	used_name_ps = "";
	ps_used = NULL;

	resource_type = 0;
}

TileMapResource::TileMapResource(char *animation_name, char *particle_name, char *particle_used_name, hgeVector pos, short xw, short yw, short layer)
: TileMapAnimationEx(animation_name, particle_name, pos, xw, yw, layer)
{
	Object::TYPE_ID = TILE_MAP_RESOURCE_ID;
	
	if (particle_used_name != NULL)
	{
		used_name_ps = particle_used_name;

		ps_used = new hgeParticleSystem( *(resources_manager->GetParticleSystem( particle_used_name )) );
	}
	else
	{
		ps_used = NULL;
	}

	used = false;

	resource_type = 0;
}

TileMapResource::~TileMapResource()
{
}


void TileMapResource::UpdateGraphic()
{
	if (!used)
	{
		TileMapAnimationEx::UpdateGraphic();
	}
	else
	{
		float dt = hge->Timer_GetDelta();
		if (ps_used)
			ps_used->Update(dt);

		
		time2 = GetTime();
		
		float tmp = (time1+TIME_REFRESH_RESOURCE);

		if (tmp < time2)
		{
			Used(false);
			time1 = time2 = 0;
		}
	}
}

void TileMapResource::Render()
{
	if (!used)
	{
		TileMapAnimationEx::Render();
	}
	else
	{
		if (ps_used)
			ps_used->Render();
	}
}

void TileMapResource::MoveParticle(float x, float y, bool move)
{
	if (!used)
	{
		TileMapAnimationEx::MoveParticle(x, y, move);
	}
	else
	{
		if (ps_used)
			ps_used->MoveTo(x, y, move);
	}
}

void TileMapResource::Used(bool used)
{
	if (this->used != used)
	{
		this->used = used;

		if (!used)
		{
			animation->Play();

			if (ps_used)
				ps_used->Stop();

			if (ps)
				ps->Fire();
		}
		else
		{
			animation->Stop();

			if (ps)
				ps->Stop();			

			if (ps_used)
				ps_used->Fire();

			time1 = GetTime();
		}
	}
}

void TileMapResource::Save(std::ofstream *f)
{
	TileMapAnimationEx::Save(f);

	size_t l = used_name_ps.length();

	f->write((const char *)&resource_type, sizeof(int));

	f->write((const char *)&l, sizeof(size_t));
	f->write(used_name_ps.c_str(), l*sizeof(char));
}

void TileMapResource::Load(std::ifstream *f)
{
	TileMapAnimationEx::Load(f);

	used_name_ps.clear();

	f->read((char *)&resource_type, sizeof(int));

	size_t l = 0;
	f->read((char *)&l, sizeof(size_t));

	char *name = new char[l];
	f->read(name, l*sizeof(char));
	name[l] = '\0';

	used_name_ps = name;

	if (ps_used)
	{
		delete ps_used;
		ps_used = NULL;
	}

	if (!used_name_ps.empty())
	{
		ps_used = new hgeParticleSystem( *(resources_manager->GetParticleSystem( used_name_ps.c_str() )) );
	}

	used = false;
}