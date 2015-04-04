#include "GraphicObject.h"
#include "World.h"


HGE *GraphicObject::hge = NULL;
hgeResourceManager *GraphicObject::resources_manager = NULL;
World *GraphicObject::world = NULL;

GraphicObject::GraphicObject()

: xw(0)
, yw(0)
{
	TYPE_ID = GRAPHIC_OBJECT_ID;
	hge = hgeCreate(HGE_VERSION);

	layer = 0;

	pos.x = 0;
	pos.y = 0;
}

GraphicObject::GraphicObject(const char *texture_name, hgeVector pos, short xw, short yw, short layer)
{
	if (texture_name)
	{
		resource_name = texture_name;
	}

	TYPE_ID = GRAPHIC_OBJECT_ID;
	hge = hgeCreate(HGE_VERSION);

	this->layer = layer; 

	this->pos = pos;
	this->xw = abs(xw);
	this->yw = abs(yw);
}

GraphicObject::~GraphicObject()
{
	hge->Release();

	// world->RemoveObject(this);

	resource_name.clear();
}

void GraphicObject::InsertToWorld()
{
	world->InsertObject(this);
}