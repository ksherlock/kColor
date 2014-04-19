#include "ColorControl.h"
#include <OS.h>
#include <Window.h>

ColorControl::ColorControl(BRect r, BMessage *m):
	BControl(r, "", "", m, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW | B_NAVIGABLE),
	color(0),
	old_val(0xffffffff)
{
	mask.red = 0;
	mask.green = 0;
	mask.blue = 0;
	mask.alpha = 255;
	ResizeTo(255 + 4, 20);
	SetValue(0);
}

void ColorControl::SetMask(rgb_color rc)
{
	mask = rc;
}

void ColorControl::SetColor(uint32 c)
{
	color = c % 3; // limit range to 0 .. 2
}


inline rgb_color avg(rgb_color r1, rgb_color r2)
{
rgb_color tmp;

	tmp.red = (r1.red + r2.red) / 2;
	tmp.green = (r1.green + r2.green) / 2;
	tmp.blue = (r1.blue + r2.blue) / 2;

	return tmp;
}

void ColorControl::KeyDown(const char * keys, int32 cnt)
{
uint32 v = Value();
	switch (*keys)
	{
	case B_LEFT_ARROW:
		if (v > 0)
		{
			SetValue(v - 1);
			Draw(Bounds());
			Invoke();
		}
		break;
	case B_RIGHT_ARROW:
		if (v < 255)
		{
			SetValue(v + 1);
			Draw(Bounds());
			Invoke();
		}
		break;
	default: BControl::KeyDown(keys, cnt);
	}
}


void ColorControl::Draw(BRect r)
{
int32 i;
rgb_color c(mask);
BPoint p1;
BPoint p2;
BPoint p3;
BRect b(Bounds());
int32 v = Value();


static rgb_color g96 =  {96,96,96,255};
static rgb_color g255 = {255,255,255,255};
static rgb_color g216 = {216,216,216,255};
static rgb_color g184 = {184,184,184,255};
rgb_color focus = {0,0,229,255};//{0x00, 0xd8, 0xaa, 0xff};
rgb_color tmp;
BView *p = Parent();
rgb_color pc;

	if (p) pc = p->ViewColor();

	// create the focus color (inverted value)
	focus = mask;
	*(((uint8 *)&c) + color) = v;
	
	if (focus.red > 127) focus.red -= 127;
	else focus.red += 127;

	if (focus.green > 127) focus.green -= 127;
	else focus.green += 127;
	
	if (focus.blue > 127) focus.blue -= 127;
	else focus.blue += 127;
		
	p1.x = b.left + 2;
	p2.x = b.left + 2;
	p1.y = b.top + 2;
	p2.y = b.bottom - 2;

	for (i = 0; i < 256; i++)
	{
		*(((uint8 *)&c) + color) = i;
		SetHighColor(c);

		if (i == v)
		{
			//SetHighColor(g216);
			StrokeLine(BPoint(v + 2, b.top + 2), BPoint(v + 2, b.top + 9));
		}
		else if ( i > v && i < v + 9) // right side
		{
			//SetHighColor(g216);
			StrokeLine(BPoint(i + 2, b.top + 2), BPoint(i + 2, b.top + 9 + i - v));
		}
		else if (i < v && i > v - 9) // left side
		{
			//SetHighColor(g216);
			StrokeLine(BPoint(i + 2, b.top + 2), BPoint(i + 2, b.top + 9 + v - i));
		}
		else
			StrokeLine(p1,p2);
			
		p1.x++;
		p2.x++;
	}
	#if 0
	p1.x = b.left + 2 + v;
	p1.y = b.bottom - 5;
	p2.x = b.left + 2 + v + 6;
	p2.y = b.bottom;
	
	SetHighColor(g96);
	StrokeLine(p1,p2);
	
	p1.x =
	#endif
	
	p1.x = b.left + 2 + v;
	p1.y = b.bottom - 10;
	
	p2.x = b.left + 2 + v + 8;
	p2.y = b.bottom - 2;
	
	p3.x = b.left +2 + v - 8;
	p3.y = b.bottom - 2;
	

	if (1)//old_val != v) // don't draw if don't need to
	{
		SetHighColor(IsFocus() ? avg(focus, g255) : g255);
		StrokeLine(p1, p2);
		SetHighColor(IsFocus() ? avg(focus, g184) : g184);
		StrokeLine(p1, p3);
	
		p1.y++;
		p2.x--;
		p3.x++;
	
		SetHighColor(IsFocus() ? avg(focus, g216) : g216);
		StrokeLine(p1, p2);	
		SetHighColor(IsFocus() ? avg(focus, g96) : g96);
		StrokeLine(p1, p3);
		
		p1.y++;
		p2.x--;
		p3.x++;
		SetHighColor(IsFocus() ? focus : g216);	
		FillTriangle(p1,p2,p3);
		old_val = v;
	}
	#if 0
	FillTriangle(
		BPoint(b.left + 2 + v, b.bottom - 10),
		BPoint(b.left + 2 + v + 10, b.bottom),
		BPoint(b.left + 2 + v - 10, b.bottom));
	#endif
	
		// draw the outline..
	if (p) SetHighColor(avg(g184, pc));
	else SetHighColor(g184);
	StrokeLine(BPoint(b.left, b.bottom), BPoint(b.left, b.top));
	StrokeLine(BPoint(b.left, b.top), BPoint(b.right, b.top));
	
	if (p) SetHighColor(avg(g96, pc));
	else SetHighColor(g96);
	StrokeLine(BPoint(b.left + 1, b.bottom - 1), BPoint(b.left + 1, b.top + 1));
	StrokeLine(BPoint(b.left + 1, b.top + 1), BPoint(b.right - 1, b.top + 1));		
	
	if (p) SetHighColor(avg(g255, pc));
	else SetHighColor(g255);
	StrokeLine(BPoint(b.left + 1, b.bottom), BPoint(b.right, b.bottom));
	StrokeLine(BPoint(b.right, b.bottom), BPoint(b.right, b.top + 1));
	
	if (p) SetHighColor(avg(g216, pc));
	else SetHighColor(g216);
	StrokeLine(BPoint(b.left + 2, b.bottom - 1), BPoint(b.right - 1,b.bottom - 1));
	StrokeLine(BPoint(b.right -1, b.bottom - 1), BPoint(b.right - 1, b.top + 2));
	
}

void ColorControl::MouseDown(BPoint where)
{
thread_id id;

	MakeFocus(true);
	// convert to a color value...
	int32 x = (int32)where.x;
	x = x - 2;
	if (x < 0) x = 0;
	if (x > 255) x = 255;
	SetValue(x);
	
	id = spawn_thread(__mouse, "Mouse Watcher", B_LOW_PRIORITY, this);
	resume_thread(id);
	

#if 0
	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
	Draw(Bounds());
	Invoke();
#endif
	
}

int32 ColorControl::__mouse(void *data)
{
ColorControl *cc = (ColorControl *)data;
uint32 buttons;
BPoint point;
int32 x;
int32 old_x = -1;

	while (1)
	{
		if (!cc->Window()->Lock()) return 0;	
		cc->GetMouse(&point, &buttons);

		
		if (1 )//cc->Bounds().Contains(point))
		{
			x = (int32)point.x - 2;
			if (x < 0) x = 0;
			if (x > 255) x = 255;
			
			if (old_x != x)
			{
				cc->SetValue(x);
				cc->Invoke();
			}
			old_x = x;
		}
		cc->Window()->Unlock();
		if (!buttons) return 0;
		snooze(20000);
	
	}

	return 0;
}

#if 0
void ColorControl::MouseMoved(BPoint where, uint32 transit, BMessage *msg)
{
int32 x;

	if (IsTracking() /*&& transit == B_INSIDE_VIEW*/)
	{
		x = (int32)where.x;
		x = x-2;
		if (x < 0) x = 0;
		if (x > 255) x = 255;
		SetValue(x);
		Draw(Bounds());
		Invoke();
	}
}

void ColorControl::MouseUp(BPoint where)
{
int32 x;
	if (Bounds().Contains(where))
	{
		x = (int32)where.x;
		x = x-2;
		if (x < 0) x = 0;
		if (x > 255) x = 255;
		SetValue(x);
		Draw(Bounds());
	}
	SetTracking(false);
}
#endif