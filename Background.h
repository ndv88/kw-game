#pragma once

#include <hge.h>
#include <hgecolor.h>
#include <hgesprite.h>
#include <hgedistort.h>
#include <hgeresource.h>

#include "Object.h"

class Background: public Object
{
public:
	Background(int screen_width, int screen_height);
	virtual ~Background();

	void Initiate();

	void Update();
	void Render();

protected:
	HTEXTURE	texObjects;
	hgeSprite	*sky;
	hgeSprite	*sun;
	hgeSprite	*moon;
	hgeSprite	*glow;
	hgeSprite	*seaglow;
	hgeSprite	*star;

	hgeDistortionMesh *sea;

	hgeColor	colWhite;

	int screen_width;
	int screen_height;

	enum { NUM_STARS = 100 };
	enum { SEA_SUBDIVISION = 16 };

	
	const float SKY_HEIGHT;
	const float STARS_HEIGHT;
	const float ORBITS_RADIUS;

	DWORD skyTopColors[3];
	DWORD skyBtmColors[3];
	DWORD seaTopColors[3];
	DWORD seaBtmColors[3];
	int seq[9];

	float time;		// 0-24 hrs
	float speed;	// hrs per second

	int	  seq_id;
	float seq_residue;

	float starX[NUM_STARS];  // x
	float starY[NUM_STARS];  // y
	float starS[NUM_STARS];  // scale
	float starA[NUM_STARS];  // alpha

	float seaP[SEA_SUBDIVISION]; // phase shift array

	hgeColor colSkyTop;
	hgeColor colSkyBtm;
	hgeColor colSeaTop;
	hgeColor colSeaBtm;

	hgeColor colSun;
	hgeColor colSunGlow;
	hgeColor colMoon;
	hgeColor colMoonGlow;
	hgeColor colSeaGlow;

	float sunX, sunY, sunS, sunGlowS;
	float moonX, moonY, moonS, moonGlowS;
	float seaGlowX, seaGlowSX, seaGlowSY;


public:
	static HGE *hge;
	static hgeResourceManager *resources_manager;
};
