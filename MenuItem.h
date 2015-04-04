#pragma once

#include <hge.h>
#include <hgegui.h>
#include <hgefont.h>
#include <hgecolor.h>
#include <string>


class hgeGUIMenuItem : public hgeGUIObject
{
public:
	hgeGUIMenuItem(int id, hgeFont *fnt, HEFFECT snd, float x, float y, float delay, char *title);

	virtual void	Render();
	virtual void	Update(float dt);

	virtual void	Enter();
	virtual void	Leave();
	virtual bool	IsDone();
	virtual void	Focus(bool bFocused);
	virtual void	MouseOver(bool over);

	virtual bool	MouseLButton(bool down);
	virtual bool	KeyDown(int key);

private:
	hgeFont *font;
	std::string title;
	HEFFECT sound;

	float delay;
	
	hgeColor	color, shadow;
	hgeColor	s_color1, d_color1, s_color2, d_color2, s_shadow, d_shadow;
	
	float		s_offset, d_offset, offset;
	float		timer1, timer2;
};
