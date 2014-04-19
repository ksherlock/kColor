#include "ColorWell.h"
#include <Bitmap.h>

ColorWell::ColorWell(BRect r):
	BView(r, "", B_WILL_DRAW, B_FOLLOW_NONE)
{


}

void ColorWell::SetColor(rgb_color c)
{
	color = c;
}
void ColorWell::SetColor(uint8 r, uint8 g, uint8 b)
{
	color.red = r;
	color.green = g;
	color.blue = b;
}

rgb_color ColorWell::GetColor(void) const
{
	return color;
}

inline rgb_color avg(rgb_color r1, rgb_color r2)
{
rgb_color tmp;

	tmp.red = (r1.red + r2.red) / 2;
	tmp.green = (r1.green + r2.green) / 2;
	tmp.blue = (r1.blue + r2.blue) / 2;

	return tmp;
}

void ColorWell::Draw(BRect r)
{
static rgb_color g96 =  {96,96,96,255};
static rgb_color g255 = {255,255,255,255};
static rgb_color g216 = {216,216,216,255};
static rgb_color g184 = {184,184,184,255};

BRect b(Bounds());
BView *p;
rgb_color pc;
	
	p = Parent();
	if (p)
	{
		pc = p->ViewColor();
		SetHighColor(pc);
		FillRect(b);
	}
	
	SetHighColor(color);
	//b.InsetBy(1,1);
	FillEllipse(b);
	
	SetHighColor(p ? avg(pc, g184) : g184);
	StrokeArc(b, 45, 180);
	
	SetHighColor(p ? avg(pc, g255) : g255);
	StrokeArc(b, 225,180);
	
	SetHighColor(p ? avg(pc, g96) : g96);
	b.InsetBy(1,1);
	StrokeArc(b, 45,180);
	
	SetHighColor(p ? avg(pc, g216) : g216);
	StrokeArc(b, 225,180);

}

static inline char int2hex(unsigned int i)
{
	if (i < 10)	return i + '0';
	else return i - 10 + 'a';
}


void ColorWell::MouseDown(BPoint p)
{
BMessage msg(B_PASTE);
BBitmap *bmp;
char hex[8];
rgb_color rgb(color);


	hex[0] = '#';
	hex[1] = int2hex(rgb.red >> 4);
	hex[2] = int2hex(rgb.red & 0x0f);
	hex[3] = int2hex(rgb.green >> 4);
	hex[4] = int2hex(rgb.green & 0x0f);
	hex[5] = int2hex(rgb.blue >> 4);
	hex[6] = int2hex(rgb.blue & 0x0f);
	hex[7] = 0;
	
	
	BView *back;
	bmp = new BBitmap(BRect(0, 0, 19, 19), B_RGB32, true);
	back = new BView(BRect(0,0,19,19), "", B_WILL_DRAW, B_FOLLOW_NONE);
	bmp->Lock();
	bmp->AddChild(back);
	
	back->SetHighColor(rgb);
	back->FillRect(back->Bounds());

	back->Draw(back->Bounds());
	bmp->Unlock();

	
	msg.AddData("RGBColor", B_RGB_COLOR_TYPE, &rgb, sizeof(rgb_color));
	msg.AddData("text/plain", B_MIME_TYPE, hex, 7);
	DragMessage(&msg, bmp, BPoint(9,9));

}