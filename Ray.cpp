#include "Ray.h"

#include "Map.h"
#include "AABB.h"

Map *Ray::tiles = NULL;

bool Ray::QueryRayObject(hgeVector &result, hgeVector &p0, hgeVector &p1, GraphicDynamicObject *obj)
{
	//index
	int i0, j0;

	int layer = obj->GetLayer();

	tiles->GetTileIndex(&p0, i0, j0);
	TileMap *tile = tiles->GetTile(layer, &p0);

	if (!tile)
		return false;

	float dx = p1.x - p0.x;
	float dy = p1.y - p0.y;
	float l = sqrt(dx*dx + dy*dy);
	if(l != 0)
	{
		dx /= l;
		dy /= l;
	}
	else
	{
		return false;
	}
	
		int X = i0;
		int Y = j0;

		int step_x, step_y;
		float max_x, max_y;
		float delta_x, delta_y;

		if(dx < 0)
		{
			step_x = -1;
			max_x = ((tile->pos.x - tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / -dx;
		}
		else if(0 < dx)
		{
			step_x = 1;
			max_x = ((tile->pos.x + tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / dx;
		}
		else
		{
			step_x = 0;
			max_x = 100000000;
			delta_x = 0;
		}

		if(dy < 0)
		{
			step_y = -1;
			max_y = ((tile->pos.y - tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / -dy;
		}
		else if(0 < dy)
		{
			step_y = 1;
			max_y = ((tile->pos.y + tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / dy;					
		}
		else
		{
			step_y = 0;
			max_y = 100000000;
			delta_y = 0;
		}		
		
		float px = p0.x;
		float py = p0.y;

		
		if(TestRayVsTile(result, px, py, dx, dy, tile))
		{
			float tx = result.x;
			float ty = result.y;
			if(TestRayObject(result, p0.x, p0.y, dx, dy, obj))
			{
				float dpO = (p0.x - result.x)*dx + (p0.y - result.y)*dy;
				float dpT = (p0.x - tx)*dx + (p0.y - ty)*dy;

				
				if(dpO < dpT)
				{
					result.x = tx;
					result.y = ty;

					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				result.x = tx;
				result.y = ty;

				return false;
			}
		}		

		short e;
		TileMap *neighbor;

		int col, row;

		while( tile != NULL )
		{	
		
			row = i0;
			col = j0;
			neighbor = NULL;

			if(max_x < max_y)
			{				

				if(step_x < 0)
				{
					e = tile->edge_left;
					neighbor = tiles->GetTile(layer, i0, j0-1);
					row = i0;
					col = j0-1;
				}
				else
				{
					e = tile->edge_right;
					neighbor = tiles->GetTile(layer, i0, j0+1);
					row = i0;
					col = j0+1;
				}

				if(0 < e)
				{
					px = p0.x + max_x*dx;
					py = p0.y + max_x*dy;
					if(e == EDGE_ID_SOLID)
					{						
						float tx = px;
						float ty = py;
						if(TestRayObject(result,p0.x,p0.y,dx,dy,obj))
						{
							float dpO = (p0.x - result.x)*dx + (p0.y - result.y)*dy;
							float dpT = (p0.x - tx)*dx + (p0.y - ty)*dy;

							if(dpO < dpT)
							{
								result.x = tx;
								result.y = ty;

								return false;
							}
							else
							{
								return true;
							}
						}
						else
						{
							result.x = tx;
							result.y = ty;

							return false;
						}
					}
					else
					{						
						if(TestRayVsTile(result,px,py,dx,dy,neighbor))
						{
							float tx = result.x;
							float ty = result.y;
							if(TestRayObject(result,p0.x,p0.y,dx,dy,obj))
							{
								float dpO = (p0.x - result.x)*dx + (p0.y - result.y)*dy;
								float dpT = (p0.x - tx)*dx + (p0.y - ty)*dy;

								if(dpO < dpT)
								{
									result.x = tx;
									result.y = ty;

									return false;
								}
								else
								{
									return true;
								}
							}
							else
							{
								result.x = tx;
								result.y = ty;

								return false;
							}
						}
						else
						{
						}
					}
				}
				else
				{
				}		   

				max_x += delta_x;
				X += step_x;
			}
			else
			{			
				if(step_y < 0)
				{
					e = tile->edge_up;
					neighbor = tiles->GetTile(layer, i0-1, j0);
					row = i0-1;
					col = j0;
				}
				else
				{
					e = tile->edge_down;
					neighbor = tiles->GetTile(layer, i0+1, j0);
					row = i0+1;
					col = j0;
				}

				if(0 < e)
				{
					px = p0.x + max_y*dx;
					py = p0.y + max_y*dy;
					if(e == EDGE_ID_SOLID)
					{
						float tx = px;
						float ty = py;
						if(TestRayObject(result,p0.x,p0.y,dx,dy,obj))
						{
							float dpO = (p0.x - result.x)*dx + (p0.y - result.y)*dy;
							float dpT = (p0.x - tx)*dx + (p0.y - ty)*dy;
	
							if(dpO < dpT)
							{
								result.x = tx;
								result.y = ty;

								return false;
							}
							else
							{
								return true;
							}
						}
						else
						{
							result.x = tx;
							result.y = ty;

							return false;
						}
					}
					else
					{						
						if(TestRayVsTile(result,px,py,dx,dy,neighbor))
						{
							float tx = result.x;
							float ty = result.y;
							if(TestRayObject(result,p0.x,p0.y,dx,dy,obj))
							{
								float dpO = (p0.x - result.x)*dx + (p0.y - result.y)*dy;
								float dpT = (p0.x - tx)*dx + (p0.y - ty)*dy;

	
								if(dpO < dpT)
								{
									result.x = tx;
									result.y = ty;

									return false;
								}
								else
								{
									return true;
								}
							}
							else
							{
								result.x = tx;
								result.y = ty;

								return false;
							}
						}
						else
						{
						}
					}
				}
				else
				{
				}		   

				max_y += delta_y;
				Y += step_y;
		  	}		
			
			tile = neighbor;
			i0 = row;
			j0 = col;
		}

	if(TestRayObject(result,p0.x,p0.y,dx,dy,obj))
	{	
		return true;		
	}

	return false;	
}

bool Ray::QueryRayVsWorld(hgeVector &p0, hgeVector &p1)
{
	if (p0 == p1)
		return true;

	float rotation = 180/M_PI * atan2(p0.y, p0.x);

	hgeVector dist = p0 - p1;
	dist.Normalize();

	hgeVector dir(0, 0);
	dir += dist * rotation;
	dir.Normalize();

	TileMap *tile_p0 = tiles->GetTile(GENERAL_MAP_LAYER, &p0);
	TileMap *tile_p1 = tiles->GetTile(GENERAL_MAP_LAYER, &p1);


	if (tiles->TestPointTile(p0.x, p0.y, tile_p0) || tiles->TestPointTile(p1.x, p1.y, tile_p1))
		return false;

	float step_x = 0, step_y = 0;

	if (p0.x < p1.x)
		step_x = abs(tile_p0->GetWidth() * dir.x);
	else
		step_x = -((tile_p0->GetWidth()) * dir.x);

	if (p0.y < p1.y)
		step_y = abs(tile_p0->GetHeight() * dir.y);
	else
		step_y =  -(tile_p0->GetHeight() * dir.y);

	hgeVector p = p0;


	while (true)
	{
		p.x += step_x;
		p.y += step_y;

		TileMap *tile = tiles->GetTile(GENERAL_MAP_LAYER, &p);

		if (tile == tile_p1)
			return true;


		if (tiles->TestPointTile(p.x, p.y, tile))
			return false;

	}

}

bool Ray::TestRayObject(hgeVector &result, float px, float py, float dx, float dy, GraphicDynamicObject *obj)
{
	if(obj->GetObjectType() >= AABB_ID)
	{
		return TestRayVsAABB(result,px,py,dx,dy,(AABB *)obj);
	}

	return false;
}

bool Ray::TestRayVsAABB(hgeVector &result, float px, float py, float dx, float dy, AABB *aabb)
{

	float bx = aabb->pos.x;
	float by = aabb->pos.y;
	short xw = aabb->xw;
	short yw = aabb->yw;
	
	
	float xval, yval;
	if(px < bx)
	{
		xval = bx - xw;
	}
	else
	{
		xval = bx + xw;
	}

	if(py < by)
	{
		yval = by - yw;
	}
	else
	{
		yval = by + yw;
	}
		

	
	float t;
	float x0,y0,x1,y1;
	if(dx == 0)
	{
		if(dy == 0)
		{
			return false;
		}
		else
		{
			x0 = bx - xw;
			x1 = bx + xw;
			y0 = y1 = yval;
			t = (yval - py)/dy;
		}
	}
	else if(dy == 0)
	{
			y0 = by - yw;
			y1 = by + yw;
			x0 = x1 = xval;
			t = (xval - px)/dx;
	}
	else
	{	
		float tX = (xval - px)/dx;
		float tY = (yval - py)/dy;

		if(tX < tY)
		{
			x0 = bx - xw;
			x1 = bx + xw;
			y0 = y1 = yval;
			t = tY;
		}
		else
		{
			y0 = by - yw;
			y1 = by + yw;
			x0 = x1 = xval;
			t = tX;
		}
	}

	
	if(0 < t)
	{
		float qx = px + 100*dx;
		float qy = py + 100*dy;
		
		float areaPQ0 = (((qx-px)*(y0-py)) - ((x0-px)*(qy-py)));
		float areaPQ1 = (((qx-px)*(y1-py)) - ((x1-px)*(qy-py)));
		if( (areaPQ0 * areaPQ1) < 0)
		{
			result.x = px + t*dx;
			result.y = py + t*dy;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;
}

bool Ray::CollideRayVsTiles(hgeVector &result, int layer, hgeVector &p0, hgeVector &p1)
{
	int i0, j0;

	tiles->GetTileIndex(&p0, i0, j0);
	TileMap *tile = tiles->GetTile(0, &p0);

	if (!tile)
		return true;

	float dx = p1.x - p0.x;
	float dy = p1.y - p0.y;
	float l = sqrt(dx*dx + dy*dy);
	if(l != 0)
	{
		dx /= l;
		dy /= l;
	}
	else
	{
		return false;
	}
	

		int X = i0;
		int Y = j0;
		

		int step_x, step_y;
		float max_x, max_y;
		float delta_x, delta_y;
		if(dx < 0)
		{
			step_x = -1;
			max_x = ((tile->pos.x - tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / -dx;
		}
		else if(0 < dx)
		{
			step_x = 1;
			max_x = ((tile->pos.x + tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / dx;
		}
		else
		{
			step_x = 0;
			max_x = 100000000;
			delta_x = 0;
		}

		if(dy < 0)
		{
			step_y = -1;
			max_y = ((tile->pos.y - tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / -dy;
		}
		else if(0 < dy)
		{
			step_y = 1;
			max_y = ((tile->pos.y + tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / dy;					
		}
		else
		{
			step_y = 0;
			max_y = 100000000;
			delta_y = 0;
		}		
		
		float px = p0.x;
		float py = p0.y;
		
		
		if(TestRayVsTile(result,px,py,dx,dy,tile))
		{
			return true;	
		}		

		short e;
		TileMap	*neighbor;

		int col, row;

		while( tile != NULL )
		{	
			row = i0;
			col = j0;

			neighbor = NULL;

			if(max_x < max_y)
			{

				if(step_x < 0)
				{
					e = tile->edge_left;
					neighbor = tiles->GetTile(0, i0, j0-1);
					row = i0;
					col = j0-1;
				}
				else
				{
					e = tile->edge_right;
					neighbor = tiles->GetTile(0, i0, j0+1);
					row = i0;
					col = j0+1;
				}

				if(0 < e)
				{
					px = p0.x + max_x*dx;
					py = p0.y + max_x*dy;
					if(e == EDGE_ID_SOLID)
					{
						result.x = px;	
						result.y = py;

						return true;
					}
					else
					{						
						if(TestRayVsTile(result,px,py,dx,dy,neighbor))
						{
							return true;
						}
						else
						{
						}
					}
				}
				else
				{
				}		   
				  

				max_x += delta_x;
				X += step_x;
			}
			else
			{			
				if(step_y < 0)
				{
					e = tile->edge_up;
					neighbor = tiles->GetTile(0, i0-1, j0);
					row = i0-1;
					col = j0;
				}
				else
				{
					e = tile->edge_down;
					neighbor = tiles->GetTile(0, i0+1, j0);
					row = i0+1;
					col = j0;
				}

				if(0 < e)
				{
					px = p0.x + max_y*dx;
					py = p0.y + max_y*dy;
					if(e == EDGE_ID_SOLID)
					{
						result.x = px;	
						result.y = py;

						return true;
					}
					else
					{						
						if(TestRayVsTile(result,px,py,dx,dy,neighbor))
						{
							return true;
						}
						else
						{
						}
					}
				}
				else
				{
				}		   

				max_y += delta_y;
				Y += step_y;
		  	}		
			
			tile = neighbor;//get next tile
			i0 = row;
			j0 = col;
		}

	return false;
}

bool Ray::TestRayVsTile(hgeVector &result, float px, float py, float dx, float dy, TileMap *t)
{
	if (t)
	{
		switch (t->GetTileType())
		{
		case TILE_TYPE_EMPTY:
		break;

		case TILE_TYPE_FULL:
			return TestRayVsTileFull(result,px,py,dx,dy,t);
		break;

		case TILE_TYPE_45DEGREE_pn:
		case TILE_TYPE_45DEGREE_nn:
		case TILE_TYPE_45DEGREE_np:
		case TILE_TYPE_45DEGREE_pp:
			return TestRayVsTile45Degree(result,px,py,dx,dy,(TileMapEx *)t);
		break;

		default:
			return false;
		break;
		}
	}

	return false;
}

bool Ray::TestRayVsTileFull(hgeVector &result, float px, float py, float dx, float dy, TileMap *t)
{
	return false;
}

bool Ray::TestRayVsTile45Degree(hgeVector &out, float px, float py, float dx, float dy, TileMapEx *t)
{

	float sx = t->signx;
	float sy = t->signy;
	
	if(0 <= (sx*dx + sy*dy))
	{
		return false;
	}
	else
	{

		float ux = sx*t->xw;
		float uy = -sy*t->yw;
		
		float wx = t->pos.x - px;
		float wy = t->pos.y - py;
		
		float s = (dy*wx - dx*wy) / (dx*uy - dy*ux);

		
		
		if(abs(s) <= 1)
		{
			out.x = t->pos.x + s*ux;
			out.y = t->pos.y + s*uy;
			return true;
		}
		else
		{
			return false;
		}
			
	}
	
	return false;
}

bool Ray::CollideRayVsWorld(hgeVector &result, hgeVector &p0, hgeVector &p1, short layer, CollideWorldInfo &info)
{
	int i0, j0;

	tiles->GetTileIndex(&p0, i0, j0);
	TileMap *tile = tiles->GetTile(layer, &p0);

	if (!tile)
		return false;

	info.tile = tile;
	info.object = NULL;

	float dx = p1.x - p0.x;
	float dy = p1.y - p0.y;
	float l = sqrt(dx*dx + dy*dy);
	if(l != 0)
	{
		dx /= l;
		dy /= l;
	}
	else
	{
		return false;
	}
	
		int X = i0;
		int Y = j0;

		int step_x, step_y;
		float max_x, max_y;
		float delta_x, delta_y;
		if(dx < 0)
		{
			step_x = -1;
			max_x = ((tile->pos.x - tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / -dx;
		}
		else if(0 < dx)
		{
			step_x = 1;
			max_x = ((tile->pos.x + tile->xw) - p0.x) / dx;
			delta_x = (2*tile->xw) / dx;
		}
		else
		{
			step_x = 0;
			max_x = 100000000;
			delta_x = 0;
		}

		if(dy < 0)
		{
			step_y = -1;
			max_y = ((tile->pos.y - tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / -dy;
		}
		else if(0 < dy)
		{
			step_y = 1;
			max_y = ((tile->pos.y + tile->yw) - p0.y) / dy;
			delta_y = (2*tile->yw) / dy;					
		}
		else
		{
			step_y = 0;
			max_y = 100000000;
			delta_y = 0;
		}		
		
		float px = p0.x;
		float py = p0.y;
		
		
		if(TestRayVsTile(result, px, py, dx, dy, tile))
		{
			return true;	
		}		

		short e;
		TileMap	*neighbor;

		int col, row;

		while( tile != NULL )
		{	
			row = i0;
			col = j0;

			neighbor = NULL;

			if (0 < tile->objects.size())
			{
				hgeVector outcome(0, 0); 
				
				for (size_t i = 0; i < tile->objects.size(); i++)
				{
					if (tile->objects[i])
					{
						bool return_value = Ray::QueryRayObject(outcome, hgeVector(px, py), p1, tile->objects[i]);

						result = outcome;

						info.object = tile->objects[i];
						info.tile = NULL;

						return return_value;
					}
				}
			}

			if(max_x < max_y)
			{
				if(step_x < 0)
				{
					e = tile->edge_left;
					neighbor = tiles->GetTile(layer, i0, j0-1);
					row = i0;
					col = j0-1;

					info.object = NULL;
					info.tile = neighbor;
				}
				else
				{
					e = tile->edge_right;
					neighbor = tiles->GetTile(layer, i0, j0+1);
					row = i0;
					col = j0+1;

					info.object = NULL;
					info.tile = neighbor;
				}

				if(0 < e)
				{
					px = p0.x + max_x*dx;
					py = p0.y + max_x*dy;
					if(e == EDGE_ID_SOLID)
					{
						result.x = px;	
						result.y = py;

						return true;
					}
					else
					{						
						if(TestRayVsTile(result, px, py, dx, dy, neighbor))
						{
							return true;
						}
						else
						{
						}
					}
				}
				else
				{
				}		   

				max_x += delta_x;
				X += step_x;
			}
			else
			{
				if(step_y < 0)
				{
					e = tile->edge_up;
					neighbor = tiles->GetTile(layer, i0-1, j0);
					row = i0-1;
					col = j0;

					info.object = NULL;
					info.tile = neighbor;
				}
				else
				{
					e = tile->edge_down;
					neighbor = tiles->GetTile(layer, i0+1, j0);
					row = i0+1;
					col = j0;

					info.object = NULL;
					info.tile = neighbor;
				}

				if(0 < e)
				{
					px = p0.x + max_y*dx;
					py = p0.y + max_y*dy;
					if(e == EDGE_ID_SOLID)
					{
						result.x = px;	
						result.y = py;

						return true;
					}
					else
					{
						if(TestRayVsTile(result, px, py, dx, dy, neighbor))
						{
							return true;
						}
						else
						{
						}
					}
				}
				else
				{
				}		   

				max_y += delta_y;
				Y += step_y;
		  	}		
			
			tile = neighbor;
			i0 = row;
			j0 = col;
		}

	return false;
}