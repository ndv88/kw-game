

#include "menuitem.h"

hgeGUIMenuItem::hgeGUIMenuItem(int id, hgeFont *font, HEFFECT sound, float x, float y, float delay, char *text)
{
	
	
	this->id = id;
	this->font = font;
	this->sound = sound;
	this->delay = delay;
	this->title = text;

	color.SetHWColor(0xFFFFE060);
	shadow.SetHWColor(0x30000000);

	offset = 0.0f;
	timer1 = -1.0f;
	timer2 = -1.0f;

	bStatic = false;
	bVisible = true;
	bEnabled = true;

	float width = font->GetStringWidth( title.c_str() );
	rect.Set(x-width/2, y, x+width/2, y+font->GetHeight());
}


void hgeGUIMenuItem::Render()
{
	font->SetColor(shadow.GetHWColor());
	font->Render( rect.x1+offset+3, rect.y1+3, HGETEXT_LEFT, title.c_str() );
	font->SetColor(color.GetHWColor());
	font->Render( rect.x1-offset, rect.y1-offset, HGETEXT_LEFT, title.c_str() );
}

void hgeGUIMenuItem::Update(float dt)
{
	if(timer2 != -1.0f)
	{
		timer2 += dt;
		if(timer2 >= delay+0.1f)
		{
			color = s_color2+d_color2;
			shadow = s_shadow+d_shadow;
			offset = 0.0f;
			timer2 = -1.0f;
		}
		else
		{
			if(timer2 < delay) 
			{ 
				color=s_color2; 
				shadow=s_shadow; 
			}
			else 
			{ 
				color = s_color2 + d_color2 * (timer2-delay) * 10; 
				shadow = s_shadow + d_shadow * (timer2-delay) * 10; 
			}
		}
	}
	else if(timer1 != -1.0f)
	{
		timer1 += dt;
		if(timer1 >= 0.2f)
		{
			color = s_color1 + d_color1;
			offset = s_offset + d_offset;
			timer1 = -1.0f;
		}
		else
		{
			color = s_color1+d_color1 * timer1 * 5;
			offset = s_offset+d_offset * timer1 * 5;
		}
	}
}

void hgeGUIMenuItem::Enter()
{
	hgeColor tcolor2;

	s_color2.SetHWColor(0x00FFE060);
	tcolor2.SetHWColor(0xFFFFE060);
	d_color2=tcolor2-s_color2;

	s_shadow.SetHWColor(0x00000000);
	tcolor2.SetHWColor(0x30000000);
	d_shadow = tcolor2-s_shadow;

	timer2=0.0f;
}

void hgeGUIMenuItem::Leave()
{
	hgeColor tcolor2;

	s_color2.SetHWColor(0xFFFFE060);
	tcolor2.SetHWColor(0x00FFE060);
	d_color2=tcolor2-s_color2;

	s_shadow.SetHWColor(0x30000000);
	tcolor2.SetHWColor(0x00000000);
	d_shadow=tcolor2-s_shadow;

	timer2=0.0f;
}

bool hgeGUIMenuItem::IsDone()
{
	if(timer2==-1.0f) 
		return true;
	else 
		return false;
}

void hgeGUIMenuItem::Focus(bool bFocused)
{
	hgeColor tcolor;
	
	if(bFocused)
	{
		hge->Effect_Play(sound);
		s_color1.SetHWColor(0xFFFFE060);
		tcolor.SetHWColor(0xFFFFFFFF);
		s_offset=0;
		d_offset=4;
	}
	else
	{
		s_color1.SetHWColor(0xFFFFFFFF);
		tcolor.SetHWColor(0xFFFFE060);
		s_offset=4;
		d_offset=-4;
	}

	d_color1 = tcolor-s_color1;
	timer1 = 0.0f;
}

void hgeGUIMenuItem::MouseOver(bool over)
{
	if(over)
	{
		gui->SetFocus(id);
	}
}

bool hgeGUIMenuItem::MouseLButton(bool down)
{
	if(!down)
	{
		offset = 4;
		return true;
	}
	else 
	{
		hge->Effect_Play(sound);
		offset = 0;
		return false;
	}
}

bool hgeGUIMenuItem::KeyDown(int key)
{
	if(key == HGEK_ENTER || key == HGEK_SPACE)
	{
		MouseLButton(true);
		return MouseLButton(false);
	}

	return false;
}
