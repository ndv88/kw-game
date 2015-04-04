#include <stdio.h>

#include "TileMap.h"



TileMap::TileMap()
: GraphicObject()
{
	TYPE_ID = TILE_MAP_ID;

	edge_up = edge_down = edge_left = edge_right = EDGE_ID_OFF;

	layer = 0;
	SetTileType(TILE_TYPE_EMPTY);
}

TileMap::TileMap(char *res_name, hgeVector pos, short xw, short yw, short layer)
: GraphicObject(res_name, pos, xw, yw, layer)
{
	TYPE_ID = TILE_MAP_ID;

	edge_up = edge_down = edge_left = edge_right = EDGE_ID_OFF;

	layer = 0;

	SetTileType(TILE_TYPE_EMPTY);
}

TileMap::~TileMap()
{
}

void TileMap::SetTileType(short type)
{
	this->tile_type = type;
}

void TileMap::Render()
{
	float x0, y0, x1, y1, x2, y2, x3, y3;

	x0 = pos.x - xw; y0 = pos.y - yw;
	x1 = pos.x + xw; y1 = pos.y - yw;
	x2 = pos.x + xw; y2 = pos.y + yw;
	x3 = pos.x - xw; y3 = pos.y + yw;

	switch (tile_type)
	{
		case TILE_TYPE_EMPTY:
		case TILE_TYPE_FULL:
			{
				if (!resource_name.empty())
				{
					hgeSprite *sprite = resources_manager->GetSprite(resource_name.c_str());

					if (sprite)
					{
						sprite->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
					}
				}
			break;
			}
	}
}

void TileMap::Save(std::ofstream *f)
{
	if (f)
	{
		size_t l = 0;

		f->write((const char *)&tile_type, sizeof(short));
		f->write((const char *)&pos.x, sizeof(float));
		f->write((const char *)&pos.y, sizeof(float));
		f->write((const char *)&xw, sizeof(short));
		f->write((const char *)&yw, sizeof(short));
		f->write((const char *)&layer, sizeof(short));

		l = resource_name.size();

		f->write((const char *)&l, sizeof(size_t));
		f->write(resource_name.c_str(), l*sizeof(char));
	}
}

void TileMap::Load(std::ifstream *f)
{
	if (f)
	{
		size_t l = 0;

		f->read((char *)&tile_type, sizeof(short));
		f->read((char *)&pos.x, sizeof(float));
		f->read((char *)&pos.y, sizeof(float));
		f->read((char *)&xw, sizeof(short));
		f->read((char *)&yw, sizeof(short));		
		f->read((char *)&layer, sizeof(short));

		f->read((char *)&l, sizeof(size_t));

		resource_name.clear();
		resource_name.resize(l);

		char *name = new char[l+1];

		f->read(name, l*sizeof(char));
		name[l] = '\0';
		resource_name = name;

		delete[] name;
	}
}

void TileMap::InsertObject(GraphicDynamicObject *obj)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (obj == objects[i])
			return;
	}

	objects.push_back(obj);
}

void TileMap::RemoveObject(GraphicDynamicObject *obj)
{
	std::vector<GraphicDynamicObject *>::iterator i = objects.begin();

	while (i != objects.end())
	{
		if (*i == obj)
		{
			objects.erase(i);
			break;
		}
		i++;
	}
}