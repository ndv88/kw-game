#include "AABB.h"

#include "TileMap.h"
#include "TileMapEx.h"

#include "World.h"

AABB::AABB()
: GraphicDynamicObject()
{
	TYPE_ID = AABB_ID;
}

AABB::AABB(const char *texture_name, hgeVector pos, short xw, short yw, short layer)
: GraphicDynamicObject(texture_name, pos, xw, yw, layer)
{
	TYPE_ID = AABB_ID;

	world_offset_x = 0;
	world_offset_y = 0;
}

AABB::~AABB()
{
}

void AABB::Render()
{
	float x0, y0, x1, y1, x2, y2, x3, y3;

	x0 = pos.x - xw; y0 = pos.y - yw;
	x1 = pos.x + xw; y1 = pos.y - yw;
	x2 = pos.x + xw; y2 = pos.y + yw;
	x3 = pos.x - xw; y3 = pos.y + yw;

	if (!resource_name.empty())
	{
		hgeSprite *sprite = resources_manager->GetSprite(resource_name.c_str());

		if (sprite)
		{
			sprite->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
		}
	}
}

void AABB::CollideVsTile(TileMap *t)
{
	if (t)
	{
		hgeVector p = t->GetPos();

		float tx = p.x;
		float ty = p.y;
		float txw = t->GetWidth();
		float tyw = t->GetHeight();

		float dx = pos.x - tx;
		float px = (txw + xw) - abs(dx);

		if(0 < px)
		{
			float dy = pos.y - ty;
			float py = (tyw + yw) - abs(dy);
			
			if(0 < py)
			{
				if(px < py)
				{
					//x
					if(dx < 0)
					{
						//left
						px *= -1;
						py = 0;
					}
					else
					{
						//right
						py = 0;
					}
				}
				else
				{				
					//y
					if(dy < 0)
					{
						//up
						px = 0;
						py *= -1;
					}
					else
					{
						//down
						px = 0;
					}
				}
				Resolve(px, py, t);
			}
		}
	}
}

void AABB::CollideVsTileMap(Map *map)
{
	int row = 0, col = 0;
	hgeVector pos = this->pos;
	map->GetTileIndex(&pos, row, col);

	tile = map->GetTile(this->layer, row, col);

	if (tile)
	{

		tile->InsertObject(this);


		float tx = (tile->GetPos().x - map->GetOffsetX());
		float ty = (tile->GetPos().y - map->GetOffsetY());
		float txw = tile->GetWidth();
		float tyw = tile->GetHeight();
		

		float dx = (pos.x - tx);
		float dy = (pos.y - ty);

		float px = 0;
		float py = 0;
		short type = tile->GetTileType();

		if(0 < type)
		{
			px = (txw + this->xw) - abs(dx);
			py = (tyw + this->yw) - abs(dy);
			

			if(px < py)
			{
				//x
				if(dx < 0)
				{
					//left
					px *= -1;
					py = 0;
				}
				else
				{
					//right
					py = 0;
				}
			}
			else
			{
				//y
				if(dy < 0)
				{
					//up
					px = 0;
					py *= -1;
				}
				else
				{

					px = 0;
				}
			}
					

			Resolve(px, py, tile);

		}

			short cross_v = 0;
			short hit_v = 0;

			dy = (pos.y - ty);
			py = (abs(dy) + this->yw) - tyw;
			

			short eV = EDGE_ID_OFF; 
			TileMap	*neighbor_v = NULL; 
			float pv = 0;
			float signy = 0;

			if(0 < py)
			{
				cross_v = 1;
				


				if(dy < 0)
				{
					eV = tile->edge_up;
					neighbor_v = map->GetTile(layer, (row-1), col);
					pv = py;
					signy = 1;
				}
				else
				{
					eV = tile->edge_down;
					neighbor_v = map->GetTile(layer, (row+1), col);
					pv = -py;
					signy = -1;
				}

				if(0 < eV)
				{
					if(eV == EDGE_ID_SOLID)
					{
						hit_v = COLLISION_AXIS;
						ReportCollision(0, pv, 0, signy, neighbor_v);
					}
					else
					{
						hit_v = Resolve(0, pv, neighbor_v);

					}
				}
			}

			
			short cross_h = 0;
			short hit_h = 0;
			
			dx = (pos.x - tx);
			px = (abs(dx) + this->xw) - txw;

			
			short eH = EDGE_ID_OFF;
			TileMap *neighbor_h = NULL;
			float ph = 0; 
			float signx = 0;

			if(0 < px)
			{
				cross_h = 1;
				


				if(dx < 0)
				{
					eH = tile->edge_left;
					neighbor_h = map->GetTile(layer, row, (col-1));
					ph = px;
					signx = 1;
				}
				else
				{
					eH = tile->edge_right;
					neighbor_h = map->GetTile(layer, row, (col+1));
					ph = -px;
					signx = -1;
				}
			
				if(0 < eH)
				{
					if(eH == EDGE_ID_SOLID)
					{
						hit_h = COLLISION_AXIS;
						ReportCollision(ph, 0, signx, 0, neighbor_h);
					}
					else
					{
						hit_h = Resolve(ph, 0, neighbor_h);
					}
				}
			}		
			

			if( (cross_h) && (hit_h != COLLISION_AXIS) && (cross_v) && (hit_v != COLLISION_AXIS) )
			{	
				float dx = (pos.x - tx);
				float dy = (pos.y - ty);
				float px = (abs(dx) + this->xw) - txw;
				float py = (abs(dy) + this->yw) - tyw;
				
				float rx = 0;
				float ry = 0;
				short hit = 0;


				TileMap *diagonal_tile;

				if((dx < 0) && (dy < 0))
				{
					eH = map->GetTile(layer, (row-1), col)->edge_left;
					eV = map->GetTile(layer, row, (col-1))->edge_up;
					diagonal_tile = map->GetTile(layer, (row-1), (col-1));
					
				}
				else if((dx < 0) && (0 < dy))
				{					
					eH = map->GetTile(layer, (row+1), col)->edge_left;
					eV = map->GetTile(layer, row, (col-1))->edge_down;
					diagonal_tile = map->GetTile(layer, (row+1), (col-1));
					
				}			
				else if((0 < dx) && (0 < dy))
				{
					eH = map->GetTile(layer, (row+1), col)->edge_right;
					eV = map->GetTile(layer, row, (col+1))->edge_down;
					diagonal_tile = map->GetTile(layer, (row+1), (col+1));
				}			
				else if((0 < dx) && (dy < 0))
				{					
					eH = map->GetTile(layer, (row-1), col)->edge_right;
					eV = map->GetTile(layer, row, (col+1))->edge_up;
					diagonal_tile = map->GetTile(layer, (row-1), (col+1));
				}
				else
				{
				}
				
				float nx, ny;
				if(px < py)
				{
					ry = ny = 0;
					if(dx < 0)
					{
						rx = px;
						nx = 1;
					}
					else
					{
						rx = -px;
						nx = -1;
					}				
				}
				else
				{
					rx = nx = 0;
					if(dy < 0)
					{
						ry = py;
						ny = 1;
					}
					else
					{
						ry = -py;
						ny = -1;
					}
				}

				if(0 < eH)
				{

					if(0 < eV)
					{
						if(eH == EDGE_ID_SOLID)
						{
							if(eV == EDGE_ID_SOLID)
							{
								ReportCollision(rx, ry, nx, ny, diagonal_tile);
							}
							else
							{
								short temp = Resolve(rx, ry, diagonal_tile);

								if(temp == COLLISION_NONE)
								{
									ReportCollision(ph, 0, signx, 0, diagonal_tile);
								}
								
							}
						}
						else
						{
							if(eV == EDGE_ID_SOLID)
							{
								short temp = Resolve(rx, ry, diagonal_tile);

								if(temp == COLLISION_NONE)
								{
									ReportCollision(0, pv, 0, signy, diagonal_tile);
								}
							}
							else
							{
								Resolve(rx, ry, diagonal_tile);
							}
						}
					}
					else
					{
						if(eH == EDGE_ID_SOLID)
						{
							ReportCollision(ph, 0, signx, 0, diagonal_tile);
						}
						else
						{
							Resolve(rx, ry, diagonal_tile);
						}
					}
				}
				else if(0 < eV)
				{
					if(eV == EDGE_ID_SOLID)
					{
						ReportCollision(0, pv, 0, signy, diagonal_tile);
					}
					else
					{
						Resolve(rx, ry, diagonal_tile);
					}							
				}
				else
				{
				}

			}
		}
}

int AABB::Resolve(float x, float y, TileMap *t)
{
	if (t)
	{
		switch (t->GetTileType())
		{
		case TILE_TYPE_EMPTY:
		break;

		case TILE_TYPE_FULL:
			return CollideVsTileFull(x, y, t);
		break;

		case TILE_TYPE_22DEGREE_pn:
		case TILE_TYPE_22DEGREE_nn:
		case TILE_TYPE_22DEGREE_np:
		case TILE_TYPE_22DEGREE_pp:
			return CollideVsTile22Degree(x, y, (TileMapEx *)t);
		break;

		case TILE_TYPE_45DEGREE_pn:
		case TILE_TYPE_45DEGREE_nn:
		case TILE_TYPE_45DEGREE_np:
		case TILE_TYPE_45DEGREE_pp:
			return CollideVsTile45Degree(x, y, (TileMapEx *)t);
		break;
		}
	}

	return COLLISION_NONE;
}

int AABB::CollideVsTileFull(float x, float y, TileMap *t)
{
	float l = sqrt(x*x + y*y);

	ReportCollision(x, y, x/l, y/l, t);

	return COLLISION_AXIS;
}

int AABB::CollideVsTile45Degree(float x, float y, TileMapEx *t)
{
	hgeVector tp = t->GetPos();

	tp.x -= world->GetMap()->GetOffsetX();
	tp.y -= world->GetMap()->GetOffsetY();

	float signx = t->signx;
	float signy = t->signy;

	float ox = (pos.x - (signx * xw)) - tp.x;
	float oy = (pos.y - (signy * yw)) - tp.y;

	float sx = t->sx;
	float sy = t->sy;
		

	float dp = (ox*sx) + (oy*sy);
	if(dp < 0)
	{
		sx *= -dp;
		sy *= -dp;		
		
		float len_n = sqrt(sx*sx + sy*sy);
		float len_p = sqrt(x*x + y*y);

		if(len_p < len_n)
		{
			ReportCollision(x, y, x/len_p, y/len_p, t);

			return COLLISION_AXIS;
		}
		else
		{
			ReportCollision(sx, sy, t->sx, t->sy, t);

			return COLLISION_OTHER;
		}
	}

	return COLLISION_NONE;
}

int AABB::CollideVsTile22Degree(float x, float y, TileMapEx *t)
{
	hgeVector tp = t->GetPos();

	tp.x -= world->GetMap()->GetOffsetX();
	tp.y -= world->GetMap()->GetOffsetY();

	short txw = t->GetWidth();
	short tyw = t->GetHeight();
	float signx = t->signx;
	float signy = t->signy;
	

	float py = pos.y - (signy*yw);
	float pen_y = tp.y - py;

	if(0 < (pen_y*signy))
	{
		
		float ox = (pos.x - (signx*xw)) - (tp.x + (signx*txw));
		float oy = (pos.y - (signy*yw)) - (tp.y - (signy*tyw));
													
		float sx = t->sx;
		float sy = t->sx;
		

		float dp = (ox*sx) + (oy*sy);
		if(dp < 0)
		{
			sx *= -dp;
			sy *= -dp;		

			float len_n = sqrt(sx*sx + sy*sy);
			float len_p = sqrt(x*x + y*y);
			
			float ay = abs(pen_y);
			if(len_p < len_n)
			{
				if(ay < len_p)
				{
					ReportCollision(0, pen_y, 0, pen_y/ay, t);
					
					return COLLISION_OTHER;
				}
				else
				{
					ReportCollision(x, y, x/len_p, y/len_p, t);
					
					return COLLISION_AXIS;
				}
			}
			else
			{
				if(ay < len_n)
				{
					ReportCollision(0, pen_y, 0, pen_y/ay, t);
					
					return COLLISION_OTHER;
				}
				else
				{
					ReportCollision(sx, sy, t->sx, t->sy, t);

					return COLLISION_OTHER;
				}
			}
		}
	}
	
	return COLLISION_NONE;
}

int AABB::Update()
{
	if (tile)
	{
		tile->RemoveObject(this);
		tile = NULL;
	}

	world_offset_x = world->GetMap()->GetOffsetX();
	world_offset_y = world->GetMap()->GetOffsetY();

	Verlet();

	CollideVsTileMap(world->GetMap());

	return -1;
}
