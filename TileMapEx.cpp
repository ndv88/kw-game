#include <stdio.h>
#include "TileMapEx.h"

TileMapEx::TileMapEx()
: TileMap()
{
	TYPE_ID = TILE_MAP_EXTENDED_ID;
}

TileMapEx::TileMapEx(char *texture_name, hgeVector pos, short xw, short yw, short layer)
: TileMap(texture_name, pos, xw, yw, layer)
{
	TYPE_ID = TILE_MAP_EXTENDED_ID;
}

TileMapEx::~TileMapEx()
{
}

void TileMapEx::SetTileType(short type)
{
	tile_type = type;
	switch (tile_type)
	{
		case TILE_TYPE_EMPTY:
		case TILE_TYPE_FULL:
			{
			signx = 0;
			signy = 0;
			sx = 0;
			sy = 0;
			}
			break;

		case TILE_TYPE_45DEGREE_pn:
			{
			signx = 1;
			signy = -1;
			sx = signx / sqrt(2.0f);//get slope unit normal
			sy = signy / sqrt(2.0f);//since normal is (1,-1), length is sqrt(1*1 + -1*-1) = sqrt(2)
			}
			break;

		case TILE_TYPE_45DEGREE_nn:
			{
			signx = -1;
			signy = -1;
			sx = signx / sqrt(2.0f);//get slope unit normal
			sy = signy / sqrt(2.0f);//since normal is (1,-1), length is sqrt(1*1 + -1*-1) = sqrt(2)
			}
			break;

		case TILE_TYPE_45DEGREE_np:
			{
			signx = -1;
			signy = 1;
			sx = signx / sqrt(2.0f);//get slope unit normal
			sy = signy / sqrt(2.0f);//since normal is (1,-1), length is sqrt(1*1 + -1*-1) = sqrt(2)
			}
			break;

		case TILE_TYPE_45DEGREE_pp:
			{
			signx = 1;
			signy = 1;
			sx = signx / sqrt(2.0f);//get slope unit normal
			sy = signy / sqrt(2.0f);//since normal is (1,-1), length is sqrt(1*1 + -1*-1) = sqrt(2)				
			}
			break;

		case TILE_TYPE_22DEGREE_pn:
			{
			signx = 1;
			signy = -1;
			float slen = sqrt(2.0f*2.0f + 1.0f*1.0f);
			sx = (signx * 1) / slen;
			sy = (signy * 2) / slen;
			}
			break;

		case TILE_TYPE_22DEGREE_nn:
			{
			signx = -1;
			signy = -1;
			float slen = sqrt(2.0f*2.0f + 1.0f*1.0f);
			sx = (signx * 1) / slen;
			sy = (signy * 2) / slen;
			}
			break;

		case TILE_TYPE_22DEGREE_np:
			{
			signx = -1;
			signy = 1;
			float slen = sqrt(2.0f*2.0f + 1.0f*1.0f);
			sx = (signx * 1) / slen;
			sy = (signy * 2) / slen;
			}
			break;

		case TILE_TYPE_22DEGREE_pp:
			{
			signx = 1;
			signy = 1;
			float slen = sqrt(2.0f*2.0f + 1.0f*1.0f);
			sx = (signx * 1) / slen;
			sy = (signy * 2) / slen;
			}
			break;
	}
}

//TODO: render
void TileMapEx::Render()
{
	switch (tile_type)
	{
		case TILE_TYPE_EMPTY:
		case TILE_TYPE_FULL:
			{
				TileMap::Render();
			}
			break;

		default:

			TileMap::Render();
			float x0, y0, x1, y1, x2, y2, x3, y3;

			x0 = pos.x - xw; y0 = pos.y - yw;
			x1 = pos.x + xw; y1 = pos.y - yw;
			x2 = pos.x + xw; y2 = pos.y + yw;
			x3 = pos.x - xw; y3 = pos.y + yw;

			hgeSprite *sprite = resources_manager->GetSprite(resource_name.c_str());
			if (sprite)
			{
				sprite->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
			}

		break;
	}
}

void TileMapEx::Save(std::ofstream *f)
{
	TileMap::Save(f);

	f->write((const char *)&signx, sizeof(float));
	f->write((const char *)&signy, sizeof(float));
	f->write((const char *)&sx, sizeof(float));
	f->write((const char *)&sy, sizeof(float));

}

void TileMapEx::Load(std::ifstream *f)
{
	TileMap::Load(f);

	f->read((char *)&signx, sizeof(float));
	f->read((char *)&signy, sizeof(float));
	f->read((char *)&sx, sizeof(float));
	f->read((char *)&sy, sizeof(float));

}