#include "Background.h"


HGE *Background::hge = NULL;
hgeResourceManager *Background::resources_manager = NULL;

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

Background::Background(int screen_w, int screen_h)
: screen_width(screen_w)
, screen_height(screen_h)

, SKY_HEIGHT(SCREEN_HEIGHT*0.6f)
, STARS_HEIGHT(SKY_HEIGHT*0.9f)
, ORBITS_RADIUS(screen_w*0.43f)
{
	TYPE_ID = BACKGROUND_ID;

	hge = hgeCreate(HGE_VERSION);

	skyTopColors[0] = 0xFF15092A;
	skyTopColors[1] = 0xFF6C6480;
	skyTopColors[2] = 0xFF89B9D0;

	skyBtmColors[0] = 0xFF303E57;
	skyBtmColors[1] = 0xFFAC7963;
	skyBtmColors[2] = 0xFFCAD7DB;

	seaTopColors[0] = 0xFF3D546B;
	seaTopColors[1] = 0xFF927E76; 
	seaTopColors[2] = 0xFF86A2AD;

	seaBtmColors[0] = 0xFF1E394C;
	seaBtmColors[1] = 0xFF2F4E64;
	seaBtmColors[2] = 0xFF2F4E64;

	seq[0] = 0;
	seq[1] = 0;
	seq[2] = 1;
	seq[3] = 2;
	seq[4] = 2;
	seq[5] = 2;
	seq[6] = 1;
	seq[7] = 0;
	seq[8] = 0;

	seaglow = NULL;
	glow = NULL;
	star = NULL;
	moon = NULL;
	sun = NULL;

	sky = NULL;
	sea = NULL;
}

Background::~Background()
{
	delete seaglow;
	delete glow;
	delete star;
	delete moon;
	delete sun;
		
	delete sky;
	delete sea;


	hge->Texture_Free(texObjects);
}

void Background::Initiate()
{
	char path[MAX_PATH];

	strcpy(path, ".\\data\\images\\simulation\\objects.png");

	texObjects = hge->Texture_Load(path);

	// Create sprites

	sky=new hgeSprite(0, 0, 0, SCREEN_WIDTH, SKY_HEIGHT);
	sea=new hgeDistortionMesh(SEA_SUBDIVISION, SEA_SUBDIVISION);
	sea->SetTextureRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-SKY_HEIGHT);

	sun=new hgeSprite(texObjects,81,0,114,114);
	sun->SetHotSpot(57,57);
	moon=new hgeSprite(texObjects,0,0,81,81);
	moon->SetHotSpot(40,40);
	star=new hgeSprite(texObjects,72,81,9,9);
	star->SetHotSpot(5,5);

	glow=new hgeSprite(texObjects,128,128,128,128);
	glow->SetHotSpot(64,64);
	glow->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE);
	seaglow=new hgeSprite(texObjects,128,224,128,32);
	seaglow->SetHotSpot(64,0);
	seaglow->SetBlendMode(BLEND_COLORADD | BLEND_ALPHAADD | BLEND_NOZWRITE);

	// Initialize simulation state

	colWhite.SetHWColor(0xFFFFFFFF);
	time=GetTime();
	//speed=0.0f;
	speed = 0.1f;

	for(int i=0;i<NUM_STARS;i++) // star positions
	{
		starX[i]=hge->Random_Float(0, SCREEN_WIDTH);
		starY[i]=hge->Random_Float(0, STARS_HEIGHT);
		starS[i]=hge->Random_Float(0.1f, 0.7f);
	}

	for(int i=0;i<SEA_SUBDIVISION;i++) // sea waves phase shifts
	{
		seaP[i]=i+hge->Random_Float(-15.0f, 15.0f);
	}
}


void Background::Update()
{
	int i, j, k;
	float zenith, a, dy, fTime;
	float posX, s1, s2;
	const float cellw=SCREEN_WIDTH/(SEA_SUBDIVISION-1);
	hgeColor col1, col2;
	DWORD dwCol1, dwCol2;

	// Update time of day

	if(speed==0.0f) time=GetTime();
	else
	{
		time+=hge->Timer_GetDelta()*speed;
		if(time>=24.0f) time-=24.0f;
	}

	seq_id=(int)(time/3);
	seq_residue=time/3-seq_id;
	zenith=-(time/12.0f*M_PI-M_PI_2);

	// Interpolate sea and sky colors

	col1.SetHWColor(skyTopColors[seq[seq_id]]);
	col2.SetHWColor(skyTopColors[seq[seq_id+1]]);
	colSkyTop=col2*seq_residue + col1*(1.0f-seq_residue);

	col1.SetHWColor(skyBtmColors[seq[seq_id]]);
	col2.SetHWColor(skyBtmColors[seq[seq_id+1]]);
	colSkyBtm=col2*seq_residue + col1*(1.0f-seq_residue);
	
	col1.SetHWColor(seaTopColors[seq[seq_id]]);
	col2.SetHWColor(seaTopColors[seq[seq_id+1]]);
	colSeaTop=col2*seq_residue + col1*(1.0f-seq_residue);

	col1.SetHWColor(seaBtmColors[seq[seq_id]]);
	col2.SetHWColor(seaBtmColors[seq[seq_id+1]]);
	colSeaBtm=col2*seq_residue + col1*(1.0f-seq_residue);

	// Update stars

	if(seq_id>=6 || seq_id<2)
		for(int i=0; i<NUM_STARS; i++)
		{
			a=1.0f-starY[i]/STARS_HEIGHT;
			a*=hge->Random_Float(0.6f, 1.0f);
			if(seq_id>=6) a*=sinf((time-18.0f)/6.0f*M_PI_2);
			else a*=sinf((1.0f-time/6.0f)*M_PI_2);
			starA[i]=a;
		}

	// Calculate sun position, scale and colors

	     if(seq_id==2) a=sinf(seq_residue*M_PI_2);
	else if(seq_id==5) a=cosf(seq_residue*M_PI_2);
	else if(seq_id>2 && seq_id<5) a=1.0f;
	else a=0.0f;
	
	colSun.SetHWColor(0xFFEAE1BE);
	colSun=colSun*(1-a)+colWhite*a;

	a=(cosf(time/6.0f*M_PI)+1.0f)/2.0f;
	if(seq_id>=2 && seq_id<=6)
	{
		colSunGlow=colWhite*a;
		colSunGlow.a=1.0f;
	}
	else colSunGlow.SetHWColor(0xFF000000);

	sunX=SCREEN_WIDTH*0.5f+cosf(zenith)*ORBITS_RADIUS;
	sunY=SKY_HEIGHT*1.2f+sinf(zenith)*ORBITS_RADIUS;
	sunS=1.0f-0.3f*sinf((time-6.0f)/12.0f*M_PI);
	sunGlowS=3.0f*(1.0f-a)+3.0f;

	// Calculate moon position, scale and colors

	if(seq_id>=6) a=sinf((time-18.0f)/6.0f*M_PI_2);
	else a=sinf((1.0f-time/6.0f)*M_PI_2);
	colMoon.SetHWColor(0x20FFFFFF);
	colMoon=colMoon*(1-a)+colWhite*a;

	colMoonGlow=colWhite;
	colMoonGlow.a=0.5f*a;

	moonX=SCREEN_WIDTH*0.5f+cosf(zenith-M_PI)*ORBITS_RADIUS;
	moonY=SKY_HEIGHT*1.2f+sinf(zenith-M_PI)*ORBITS_RADIUS;
	moonS=1.0f-0.3f*sinf((time+6.0f)/12.0f*M_PI);
	moonGlowS=a*0.4f+0.5f;

	// Calculate sea glow

	if(time>19.0f || time<4.5f) // moon
	{
		a=0.2f; // intensity
		if(time>19.0f && time<20.0f) a*=(time-19.0f);
		else if(time>3.5f && time<4.5f) a*=1.0f-(time-3.5f);

		colSeaGlow=colMoonGlow;
		colSeaGlow.a=a;
		seaGlowX=moonX;
		seaGlowSX=moonGlowS*3.0f;
		seaGlowSY=moonGlowS*2.0f;
	}
	else if(time>6.5f && time<19.0f) // sun
	{
		a=0.3f; // intensity
		if(time<7.5f) a*=(time-6.5f);
		else if(time>18.0f) a*=1.0f-(time-18.0f);

		colSeaGlow=colSunGlow;
		colSeaGlow.a=a;
		seaGlowX=sunX;
		seaGlowSX=sunGlowS;
		seaGlowSY=sunGlowS*0.6f;
	}
	else colSeaGlow.a=0.0f;

	// Move waves and update sea color

	for(i=1; i<SEA_SUBDIVISION-1; i++)
	{
		a=float(i)/(SEA_SUBDIVISION-1);
		col1=colSeaTop*(1-a)+colSeaBtm*a;
		dwCol1=col1.GetHWColor();
		fTime=2.0f*hge->Timer_GetTime();
		a*=20;

		for(j=0; j<SEA_SUBDIVISION; j++)
		{
			sea->SetColor(j, i, dwCol1);

			dy=a*sinf(seaP[i]+(float(j)/(SEA_SUBDIVISION-1)-0.5f)*M_PI*16.0f-fTime);
			sea->SetDisplacement(j, i, 0.0f, dy, HGEDISP_NODE);
		}
	}

	dwCol1=colSeaTop.GetHWColor();
	dwCol2=colSeaBtm.GetHWColor();

	for(j=0; j<SEA_SUBDIVISION; j++)
	{
		sea->SetColor(j, 0, dwCol1);
		sea->SetColor(j, SEA_SUBDIVISION-1, dwCol2);
	}

	// Calculate light path

	if(time>19.0f || time<5.0f) // moon
	{
		a=0.12f; // intensity
		if(time>19.0f && time<20.0f) a*=(time-19.0f);
		else if(time>4.0f && time<5.0f) a*=1.0f-(time-4.0f);
		posX=moonX;
	}
	else if(time>7.0f && time<17.0f) // sun
	{
		a=0.14f; // intensity
		if(time<8.0f) a*=(time-7.0f);
		else if(time>16.0f) a*=1.0f-(time-16.0f);
		posX=sunX;
	}
	else a=0.0f;

	if(a!=0.0f)
	{
		k=(int)floorf(posX/cellw);
		s1=(1.0f-(posX-k*cellw)/cellw);
		s2=(1.0f-((k+1)*cellw-posX)/cellw);

		if(s1>0.7f) s1=0.7f;
		if(s2>0.7f) s2=0.7f;

		s1*=a;
		s2*=a;
	
		for(i=0; i<SEA_SUBDIVISION; i+=2)
		{
			a=sinf(float(i)/(SEA_SUBDIVISION-1)*M_PI_2);

			col1.SetHWColor(sea->GetColor(k,i));
			col1+=colSun*s1*(1-a);
			col1.Clamp();
			sea->SetColor(k, i, col1.GetHWColor());
			
			col1.SetHWColor(sea->GetColor(k+1,i));
			col1+=colSun*s2*(1-a);
			col1.Clamp();
			sea->SetColor(k+1, i, col1.GetHWColor());
		}
	}
}

void Background::Render()
{
	sky->SetColor(colSkyTop.GetHWColor(), 0);
	sky->SetColor(colSkyTop.GetHWColor(), 1);
	sky->SetColor(colSkyBtm.GetHWColor(), 2);
	sky->SetColor(colSkyBtm.GetHWColor(), 3);
	sky->Render(0, 0);

	// Render stars

	if(seq_id>=6 || seq_id<2)
		for(int i=0; i<NUM_STARS; i++)
		{
			star->SetColor((DWORD(starA[i]*255.0f)<<24) | 0xFFFFFF);
			star->RenderEx(starX[i], starY[i], 0.0f, starS[i]);
		}

	// Render sun
	
	glow->SetColor(colSunGlow.GetHWColor());
	glow->RenderEx(sunX, sunY, 0.0f, sunGlowS);
	sun->SetColor(colSun.GetHWColor());
	sun->RenderEx(sunX, sunY, 0.0f, sunS);

	// Render moon

	glow->SetColor(colMoonGlow.GetHWColor());
	glow->RenderEx(moonX, moonY, 0.0f, moonGlowS);
	moon->SetColor(colMoon.GetHWColor());
	moon->RenderEx(moonX, moonY, 0.0f, moonS);

	// Render sea

	sea->Render(0, SKY_HEIGHT);
	seaglow->SetColor(colSeaGlow.GetHWColor());
	seaglow->RenderEx(seaGlowX, SKY_HEIGHT, 0.0f, seaGlowSX, seaGlowSY);
}
