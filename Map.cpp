#include <fstream>

#include "Map.h"

Map::Map(size_t layers, size_t rows, size_t cols, int screen_width, int screen_hight, short tile_width, short tile_hight)
{
	TYPE_ID = MAP_ID;

	this->layers = layers;

	this->rows = rows;
	this->cols = cols;

	this->screen_width = screen_width;
	this->screen_hight = screen_hight;

	this->tile_width = tile_width;
	this->tile_hight = tile_hight;

	this->offset_x = 0;
	this->offset_y = 0;

	grid.resize(layers);

	for (size_t l = 0; l < layers; l++)
	{
		grid[l].resize(rows);
		for (size_t i = 0; i < rows; i++)
		{
			grid[l][i].resize(cols);
		}
	}
}

Map::~Map()
{
	Free();
}

void Map::Free()
{
	for (size_t l = 0; l < layers; l++)
	{
		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
			{
				if (grid[l][i][j] != NULL)
				{

					if (TILE_MAP_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMap *)grid[l][i][j]);
					else if (TILE_MAP_EXTENDED_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMapEx *)grid[l][i][j]);
					else if (TILE_MAP_ANIMATION_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMapAnimation *)grid[l][i][j]);
					else if (TILE_MAP_PARTICLE_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMapParticle *)grid[l][i][j]);
					else if (TILE_MAP_ANIMATION_EX_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMapAnimationEx *)grid[l][i][j]);
					else if (TILE_MAP_RESOURCE_ID == grid[l][i][j]->GetObjectType())
						delete ((TileMapResource *)grid[l][i][j]);

					


					//delete grid[l][i][j];

					grid[l][i][j] = NULL;
				}
			}
		}
	}
	grid.clear();
}

void Map::InsertTile(int row, int col, TileMap *tile)
{
	size_t layer = tile->GetLayer();

	grid[layer][row][col] = tile;
}

TileMap* Map::GetTile(int layer, const hgeVector *pos)
{
	return GetTile(layer, pos->x, pos->y);
}

TileMap* Map::GetTile(int layer, float x, float y)
{
	int row = (int)floor((y + offset_y) / tile_hight);
	int col = (int)floor((x + offset_x) / tile_width);

	if (row < 0 || row >= rows)
		return NULL;
	if (col < 0 || col >= cols)
		return NULL;

	return grid[layer][row][col];
}

TileMap* Map::GetTile(int layer, int row, int col)
{
	if (row < 0 || row >= rows)
		return NULL;
	if (col < 0 || col >= cols)
		return NULL;

	
	return grid[layer][row][col];
}

void Map::GetTileIndex(const hgeVector *pos, int &row, int &col)
{
	row = (int)floor((pos->y + offset_y) / tile_hight);
	col = (int)floor((pos->x + offset_x) / tile_width);
}

bool Map::HorizontalScroll(int offset_x)
{
	float old_offset = this->offset_x;
	if (0 < offset_x)
	{
		if (((this->offset_x + offset_x) + screen_width) < ((tile_width * cols)))
		{
			this->offset_x += offset_x;
		}
		else
		{
			this->offset_x = (cols * tile_width) - screen_width;
		}
	}
	else if (0 > offset_x)
	{
		if ((this->offset_x + offset_x) > 0)
			this->offset_x += offset_x;
		else
			this->offset_x = 0;
	}

	if (old_offset != this->offset_x)
		return true;
	else
		return false;
}

bool Map::VerticalScroll(int offset_y)
{
	float old_offset = this->offset_y;

	if (0 < offset_y)
	{
		if (((this->offset_y + offset_y) + screen_hight) < ((tile_hight * rows)))
			this->offset_y += offset_y;
		else
			this->offset_y = (rows * tile_hight) - screen_hight;
	}
	else if (0 > offset_y)
	{
		if ((this->offset_y + offset_y) > 0)
			this->offset_y += offset_y;
		else
			this->offset_y = 0;
	}

	if (old_offset != this->offset_y)
		return true;
	else
		return false;
}


void Map::Render(int layer)
{
	hgeVector temp;

	size_t row = floor(offset_y / tile_hight);
	size_t col = floor(offset_x / tile_width);
	size_t n = (size_t)ceil((float)((screen_hight + offset_y) / tile_hight));
	size_t m = (size_t)ceil((float)((screen_width + offset_x) / tile_width));

	float temp_x = (tile_width / 2) - ((int)offset_x % tile_width);
	float temp_y = (tile_hight / 2) - ((int)offset_y % tile_hight);
	float x = temp_x;
	float y = temp_y;

	size_t l = layer;

	for (size_t i = row; i < n; i++)
	{
		for (size_t j = col; j < m; j++)
		{
			if (grid[l][i][j] != NULL)
			{
				temp = grid[l][i][j]->pos;

				grid[l][i][j]->pos.x = x;
				grid[l][i][j]->pos.y = y;
				
				int object_type = grid[l][i][j]->GetObjectType();

				switch( object_type )
				{
					case TILE_MAP_ID:
						((TileMap *)grid[l][i][j])->Render();
					break;

					case TILE_MAP_EXTENDED_ID:
						((TileMapEx *)grid[l][i][j])->Render();
					break;

					case TILE_MAP_ANIMATION_ID:
						((TileMapAnimation *)grid[l][i][j])->Render();
					break;

					case TILE_MAP_PARTICLE_ID:
						((TileMapParticle *)grid[l][i][j])->GetParticle()->MoveTo(x, y, true);
						((TileMapParticle *)grid[l][i][j])->Render();
					break;

					case TILE_MAP_ANIMATION_EX_ID:
						((TileMapAnimationEx *)grid[l][i][j])->MoveParticle(x, y, true);
						((TileMapAnimationEx *)grid[l][i][j])->Render();
					break;
					
					case TILE_MAP_RESOURCE_ID:
						((TileMapResource *)grid[l][i][j])->MoveParticle(x, y, true);
						((TileMapResource *)grid[l][i][j])->Render();
					break;
				}

				grid[l][i][j]->pos = temp;
			}

			x += tile_width;
		}
		x = temp_x;
		y += tile_hight;
	}
}

void Map::Save(const char *file_name)
{
	std::ofstream f(file_name, std::ios::binary);
	if (f.is_open())
	{
		f.write((const char *)&layers, sizeof(size_t));
		f.write((const char *)&rows, sizeof(size_t));
		f.write((const char *)&cols, sizeof(size_t));

		for (size_t l = 0; l < layers; l++)
		{
			for (size_t i = 0; i < rows; i++)
			{
				for (size_t j = 0; j < cols; j++)
				{

					if (grid[l][i][j] == NULL)
					{
						short null_tile = -1;
						f.write((const char *)&null_tile, sizeof(short));
					}
					else
					{
						short object_type = grid[l][i][j]->GetObjectType();

						f.write((const char *)&object_type, sizeof(object_type));

						switch (object_type)
						{

						case TILE_MAP_ID:
							((TileMap *)grid[l][i][j])->Save(&f);
						break;

						case TILE_MAP_EXTENDED_ID:
							((TileMapEx *)grid[l][i][j])->Save(&f);
						break;

						case TILE_MAP_ANIMATION_ID:
							((TileMapAnimation *)grid[l][i][j])->Save(&f);
						break;

						case TILE_MAP_PARTICLE_ID:
							((TileMapParticle *)grid[l][i][j])->Save(&f);
						break;

						case TILE_MAP_ANIMATION_EX_ID:
							((TileMapAnimationEx *)grid[l][i][j])->Save(&f);
						break;

						case TILE_MAP_RESOURCE_ID:
							((TileMapResource *)grid[l][i][j])->Save(&f);
						break;
						}
					}
					f.flush();
				}
			}
		}
		f.close();
	}
	
}

void Map::Load(const char *file_name, int &count_entitys, std::string &next_level)
{
	Free();

	std::ifstream f(file_name, std::ios::binary);

	if (f.is_open())
	{
		short object_type = 0;
		
		f.read((char *)&count_entitys, sizeof(int));

		int l = 0;
		char *buffer = NULL;
		f.read((char *)&l, sizeof(int));
		buffer = new char[l];
		f.read((char *)buffer, l);
		buffer[l] = '\0';
		next_level = buffer;
		//delete[] buffer;



		f.read((char *)&layers, sizeof(size_t));
		f.read((char *)&rows, sizeof(size_t));
		f.read((char *)&cols, sizeof(size_t));

		grid.resize(layers);

		for (size_t l = 0; l < layers; l++)
		{
			grid[l].resize(rows);
			for (size_t i = 0; i < rows; i++)
			{
				grid[l][i].resize(cols);
			}
		}

		for (size_t l = 0; l < layers; l++)
		{
			for (size_t i = 0; i < rows; i++)
			{
				for (size_t j = 0; j < cols; j++)
				{
					f.read((char *)&object_type, sizeof(short));

					switch(object_type)
					{
					case -1:
						grid[l][i][j] = NULL;
					break;

					case TILE_MAP_ID:
						grid[l][i][j] = new TileMap();
						grid[l][i][j]->Load(&f);
					break;

					case TILE_MAP_EXTENDED_ID:
						grid[l][i][j] = new TileMapEx();
						((TileMapEx *)grid[l][i][j])->Load(&f);
					break;

					case TILE_MAP_ANIMATION_ID:
						grid[l][i][j] = new TileMapAnimation();
						((TileMapAnimation *)grid[l][i][j])->Load(&f);
					break;

					case TILE_MAP_PARTICLE_ID:
						grid[l][i][j] = new TileMapParticle();
						((TileMapParticle *)grid[l][i][j])->Load(&f);
					break;

					case TILE_MAP_ANIMATION_EX_ID:
						grid[l][i][j] = new TileMapAnimationEx();
						((TileMapAnimationEx *)grid[l][i][j])->Load(&f);
					break;

					case TILE_MAP_RESOURCE_ID:
						grid[l][i][j] = new TileMapResource();
						((TileMapResource *)grid[l][i][j])->Load(&f);
					break;
					}
				}
			}
		}
		f.close();
	}


}

void Map::UpdateGraphic()
{
	size_t row = floor(offset_y / tile_hight);
	size_t col = floor(offset_x / tile_width);
	size_t n = (size_t)ceil((float)((screen_hight + offset_y) / tile_hight));
	size_t m = (size_t)ceil((float)((screen_width + offset_x) / tile_width));

	for (size_t l = 0; l < layers; l++)
	{
		for (size_t i = row; i < n; i++)
		{
			for (size_t j = col; j < m; j++)
			{
				if (grid[l][i][j] != NULL)
				{
					grid[l][i][j]->UpdateGraphic();
				}
			}
		}
	}
}



void Map::UpdateEdges(int layer)
{
	size_t l = layer;

	for (size_t i = 1; i < rows - 1; i++)
	{
		for (size_t j = 1; j < cols - 1; j++)
		{
			TileMapEx *tile;
			TileMapEx *tile_next;//Соседний tile

			tile = (TileMapEx *)grid[l][i][j];
			tile_next = (TileMapEx *)grid[l][i - 1][j];
			

			if(tile->tile_type == TILE_TYPE_EMPTY)
			{
				if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_up = EDGE_ID_OFF;
				}
				else if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_up = EDGE_ID_SOLID;
				}
				else if(((tile_next->signy * -1) <= 0))
				{
					tile->edge_up = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_up = EDGE_ID_SOLID;
				}			
			}
			else if(tile->tile_type == TILE_TYPE_FULL)
			{
				if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_up = EDGE_ID_OFF;
				}
				else if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_up = EDGE_ID_OFF;
				}
				else if((tile_next->signy * -1) <= 0)
				{
					tile->edge_up = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_up = EDGE_ID_OFF;
				}
			}
			else
			{
				if(0 <= (tile->signy * -1))
				{
					if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_up = EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_up = EDGE_ID_SOLID;
					}
					else if((tile->signy * -1) <= 0)
					{
						tile->edge_up = EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_up = EDGE_ID_SOLID;
					}
				}
				else
				{
							
					if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_up = EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_up = EDGE_ID_OFF;
					}
					else if((tile_next->signy * -1) <= 0)
					{
						tile->edge_up = EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_up = EDGE_ID_OFF;
					}
					
				}	
			}
		

			tile_next = (TileMapEx *)grid[l][i + 1][j];


			if(tile->tile_type == TILE_TYPE_EMPTY)
			{
				if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_down = EDGE_ID_OFF;
				}		
				else if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_down = EDGE_ID_SOLID;
				}
				else if((tile_next->signy * 1) <= 0)
				{
					tile->edge_down = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_down = EDGE_ID_SOLID;
				}			
			}
			else if(tile->tile_type == TILE_TYPE_FULL)
			{
				if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_down = EDGE_ID_OFF;
				}
				else if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_down = EDGE_ID_OFF;
				}		
				else if((tile_next->signy * 1) <= 0)
				{
					tile->edge_down = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_down = EDGE_ID_OFF;
				}
			}
			else
			{
				if(0 <= (tile->signy * 1))
				{
					if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_down = EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_down = EDGE_ID_SOLID;
					}
					else if((tile_next->signy * 1) <= 0)
					{
						tile->edge_down = EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_down = EDGE_ID_SOLID;
					}
				}
				else
				{
					if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_down = EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_down = EDGE_ID_OFF;
					}			
					else if((tile_next->signy * 1) <= 0)
					{
						tile->edge_down = EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_down = EDGE_ID_OFF;
					}
				}	
			}		

			tile_next = (TileMapEx *)grid[l][i][j + 1];

			if(tile->tile_type == TILE_TYPE_EMPTY)
			{
				if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_right = EDGE_ID_OFF;
				}		
				else if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_right = EDGE_ID_SOLID;
				}
				else if((tile_next->signx * 1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_pn || tile_next->tile_type == TILE_TYPE_22DEGREE_pp)
				{
					tile->edge_right = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_right = EDGE_ID_SOLID;
				}			
			}
			else if(tile->tile_type == TILE_TYPE_FULL)
			{
				if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_right = EDGE_ID_OFF;
				}
				else if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_right = EDGE_ID_OFF;
				}		
				else if((tile_next->signx * 1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_pn || tile_next->tile_type == TILE_TYPE_22DEGREE_pp)
				{
					tile->edge_right = EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_right = EDGE_ID_OFF;
				}
			}
			else
			{
				if(0 <= (tile->signx*1))
				{					
					if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_right = EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_right = EDGE_ID_SOLID;
					}
					else if((tile_next->signx * 1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_pn || tile_next->tile_type == TILE_TYPE_22DEGREE_pp)
					{
						tile->edge_right = EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_right = EDGE_ID_SOLID;
					}
				}
				else
				{
					if(tile->tile_type == TILE_TYPE_22DEGREE_nn || tile->tile_type == TILE_TYPE_22DEGREE_np)
					{

						if(tile_next->tile_type == TILE_TYPE_EMPTY)
						{
							tile->edge_right = EDGE_ID_OFF;
						}
						else if(tile_next->tile_type == TILE_TYPE_FULL)
						{
							tile->edge_right = EDGE_ID_SOLID;
						}				
						else if((tile_next->signx*1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_pn || tile_next->tile_type == TILE_TYPE_22DEGREE_pp)
						{
							tile->edge_right = EDGE_ID_INTERESTING;
						}
						else if(tile_next->tile_type == TILE_TYPE_FULL || (0 < (tile_next->signx * 1)) )
						{
							tile->edge_right = EDGE_ID_SOLID;
						}
						else
						{							
							tile->edge_right = EDGE_ID_OFF;
						}				
						
					}
					else 
					{
						if(tile_next->tile_type == TILE_TYPE_FULL)
						{
							tile->edge_right = EDGE_ID_OFF;
						}
						else if(tile_next->tile_type == TILE_TYPE_EMPTY)
						{
							tile->edge_right = EDGE_ID_OFF;
						}			
						else if((tile_next->signx * 1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_pn || tile_next->tile_type == TILE_TYPE_22DEGREE_pp)
						{
							tile->edge_right = EDGE_ID_INTERESTING;
						}
						else
						{
							tile->edge_right = EDGE_ID_OFF;
						}
					}
				}	
			}

			tile_next = (TileMapEx *)grid[l][i][j - 1];//Соседний tile

			if(tile->tile_type == TILE_TYPE_EMPTY)
			{
				if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{					
					tile->edge_left = EDGE_ID_OFF;
				}		
				else if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_left= EDGE_ID_SOLID;
				}
				else if((tile_next->signx*-1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_nn || tile_next->tile_type == TILE_TYPE_22DEGREE_np)
				{
					tile->edge_left= EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_left= EDGE_ID_SOLID;
				}			
			}
			else if(tile->tile_type == TILE_TYPE_FULL)
			{
				if(tile_next->tile_type == TILE_TYPE_FULL)
				{
					tile->edge_left = EDGE_ID_OFF;
				}
				else if(tile_next->tile_type == TILE_TYPE_EMPTY)
				{
					tile->edge_left = EDGE_ID_OFF;
				}		
				else if((tile_next->signx * -1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_nn || tile_next->tile_type == TILE_TYPE_22DEGREE_np)
				{
					tile->edge_left= EDGE_ID_INTERESTING;
				}
				else
				{
					tile->edge_left= EDGE_ID_OFF;
				}
			}
			else
			{
				if(0 <= (tile->signx * -1))
				{
					if(tile_next->tile_type == TILE_TYPE_EMPTY)
					{
						tile->edge_left= EDGE_ID_OFF;
					}
					else if(tile_next->tile_type == TILE_TYPE_FULL)
					{
						tile->edge_left= EDGE_ID_SOLID;
					}
					else if((tile_next->signx * -1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_nn || tile_next->tile_type == TILE_TYPE_22DEGREE_np)
					{
						tile->edge_left= EDGE_ID_INTERESTING;
					}
					else
					{
						tile->edge_left= EDGE_ID_SOLID;
					}
				}
				else
				{
					if(tile->tile_type == TILE_TYPE_22DEGREE_pn || tile->tile_type == TILE_TYPE_22DEGREE_pp)
					{
						if(tile_next->tile_type == TILE_TYPE_EMPTY)
						{
							tile->edge_left= EDGE_ID_OFF;
						}
						else if(tile_next->tile_type == TILE_TYPE_FULL)
						{
							tile->edge_left= EDGE_ID_SOLID;
						}
						else if((tile_next->signx*-1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_nn || tile_next->tile_type == TILE_TYPE_22DEGREE_np)
						{
							tile->edge_left= EDGE_ID_INTERESTING;
						}
						else if(0 < (tile_next->signx*-1) || tile_next->tile_type == TILE_TYPE_FULL)
						{
							tile->edge_left= EDGE_ID_SOLID;
						}
						else
						{
							tile->edge_left= EDGE_ID_OFF;
						}				
					}
					else 
					{
						if(tile_next->tile_type == TILE_TYPE_FULL)
						{
							tile->edge_left= EDGE_ID_OFF;
						}
						else if(tile_next->tile_type == TILE_TYPE_EMPTY)
						{
							tile->edge_left= EDGE_ID_OFF;
						}			
						else if((tile_next->signx*-1) <= 0 || tile_next->tile_type == TILE_TYPE_22DEGREE_nn || tile_next->tile_type == TILE_TYPE_22DEGREE_np)
						{
							tile->edge_left= EDGE_ID_INTERESTING;
						}
						else
						{
							tile->edge_left= EDGE_ID_OFF;
						}
					}
				}	
			}
		}
	}
}

bool Map::QueryPoint(int layer, float x, float y)
{
	TileMap *tile = this->GetTile(layer, x, y);
    return TestPointTile(x, y, tile);
}

bool Map::TestPointTile(float x, float y, TileMap *t)
{
	if (t)
	{
		x += offset_x;
		y += offset_y;

		switch (t->GetTileType())
		{
		case TILE_TYPE_EMPTY:
			return false;
		break;

		case TILE_TYPE_FULL:
			return TestPointVsTileFull(x, y, t);
		break;

		case TILE_TYPE_45DEGREE_pn:
		case TILE_TYPE_45DEGREE_nn:
		case TILE_TYPE_45DEGREE_np:
		case TILE_TYPE_45DEGREE_pp:
			return TestPointVsTile45Degree(x, y, (TileMapEx *)t);
		break;

		case TILE_TYPE_22DEGREE_pn:
		case TILE_TYPE_22DEGREE_nn:
		case TILE_TYPE_22DEGREE_np:
		case TILE_TYPE_22DEGREE_pp:
			return TestPointVsTile22Degree(x, y, (TileMapEx *)t);
		break;

		default:
			return false;
		break;
		}
	}

	return false;
}

bool Map::TestPointVsTileFull(float x, float y, TileMap *t)
{
	return true;
}

bool Map::TestPointVsTile45Degree(float x, float y, TileMapEx *t)
{
    float dx = x - t->pos.x;
    float dy = y - t->pos.y;
    if (dx * t->sx + dy * t->sy <= 0) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool Map::TestPointVsTile22Degree(float x, float y, TileMapEx *t)
{
	hgeVector pos = t->GetPos();
    float dx = x - (pos.x + t->signx * t->xw);
    float dy = y - (pos.y - t->signy * t->yw);
    if (dx * t->sx + dy * t->sy <= 0) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void Map::ScrollTo(hgeVector point)
{
	float _x = point.x - (screen_width/2);
	float _y = point.y - (screen_hight/2);

	HorizontalScroll(_x);
	VerticalScroll(_y);
}