#include "GraphicDynamicObject.h"

#include "TileMap.h"

GraphicDynamicObject::GraphicDynamicObject()
: GraphicObject()
{
	TYPE_ID = GRAPHIC_DYNAMIC_OBJECT_ID;

	this->old_pos = pos;
	speed = max_speed = 0;

	tile = NULL;
}

GraphicDynamicObject::GraphicDynamicObject(const char *texture_name, hgeVector pos, short xw, short yw, short layer)
: GraphicObject(texture_name, pos, xw, yw, layer)
{
	TYPE_ID = GRAPHIC_DYNAMIC_OBJECT_ID;
	
	this->old_pos = pos;
	speed = max_speed = 0;

	tile = NULL;

}

GraphicDynamicObject::~GraphicDynamicObject()
{
	if (tile)
	{
		tile->RemoveObject(this);
		tile = NULL;
	}
}

void GraphicDynamicObject::Verlet()
{
	float d = DRAGGING;
	float g = GRAVITATION;
	
	float px, py;
		
	float ox = old_pos.x; //we can't swap buffers since mcs/sticks point directly to vector2s..
	float oy = old_pos.y;

	old_pos.x = px = pos.x;		//get vector values
	old_pos.y = py = pos.y;		//pos = position  
								//old_pos = old position

	//integrate	
	pos.x += (d*px) - (d*ox);
	pos.y += (d*py) - (d*oy) + g;
}

//(px,py) is projection vector, (dx,dy) is surface normal
void GraphicDynamicObject::ReportCollision(float px, float py, float dx, float dy, TileMap *tile)
{
	//calc velocity
	float vx = pos.x - old_pos.x;
	float vy = pos.y - old_pos.y;
	
	//find component of velocity parallel to collision normal
	float dp = (vx*dx + vy*dy);
	float nx = dp*dx;//project velocity onto collision normal
	
	float ny = dp*dy;//nx,ny is normal velocity
	
	float tx = vx-nx;//px,py is tangent velocity
	float ty = vy-ny;

	//we only want to apply collision response forces if the object is travelling into, and not out of, the collision
	float b,bx,by,f,fx,fy;
	if(dp < 0)
	{
		f = FRICTION;
		fx = tx * f;
		fy = ty * f;
		
		b = 1 + BOUNCE;//this bounce constant should be else where, i.e inside the object/tile/etc..

		bx = (nx * b);
		by = (ny * b);
	}
	else
	{
		//moving out of collision, do not apply forces
		bx = by = fx = fy = 0;
	}

	pos.x += px;//project object out of collision
	pos.y += py;
	
	old_pos.x += px + bx + fx;//apply bounce+friction impulses which alter velocity
	old_pos.y += py + by + fy;
}

void GraphicDynamicObject::Move(int move)
{
	float fx = 0;
	float fy = 0;

	switch (move)
	{
	case MOVE_LEFT:
		fx -= speed;
		break;

	case MOVE_RIGHT:
		fx += speed;
		break;

	case MOVE_UP:
		fy -= speed + GRAVITATION;
		break;

	case MOVE_DOWN:
		fy += speed;
		break;
	}

	float vx = pos.x - old_pos.x;
	float newx = min(max_speed, max(-max_speed, vx+fx));
	pos.x = old_pos.x + newx;

	float vy = pos.y - old_pos.y;
	float newy = min(max_speed, max(-max_speed, vy+fy));
	pos.y = old_pos.y + newy;
}