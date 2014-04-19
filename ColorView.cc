
#include "ColorView.h"
#include <Bitmap.h>


ColorView::ColorView(BRect r):
	BView(r, "", B_WILL_DRAW, B_FOLLOW_TOP | B_FOLLOW_LEFT)
{
}


inline rgb_color avg(rgb_color r1, rgb_color r2)
{
rgb_color tmp;

	tmp.red = (r1.red + r2.red) / 2;
	tmp.green = (r1.green + r2.green) / 2;
	tmp.blue = (r1.blue + r2.blue) / 2;

	return tmp;
}

void ColorView::Draw(BRect r)
{
static rgb_color g96 =  {96,96,96,255};
static rgb_color g255 = {255,255,255,255};
static rgb_color g216 = {216,216,216,255};
static rgb_color g184 = {184,184,184,255};
BView *p = Parent();
rgb_color pc;
BRect b(Bounds());

	if (p) pc = p->ViewColor();
	
	//SetDrawingMode(B_OP_BLEND);
	if (p) SetHighColor( avg(pc, g184));
	else SetHighColor(g184);
	StrokeLine(BPoint(b.left, b.bottom), BPoint(b.left, b.top));
	StrokeLine(BPoint(b.left, b.top), BPoint(b.right, b.top));
	
	if (p) SetHighColor( avg(pc, g96));
	else SetHighColor(g96);
	StrokeLine(BPoint(b.left + 1, b.bottom - 1), BPoint(b.left + 1, b.top + 1));
	StrokeLine(BPoint(b.left + 1, b.top + 1), BPoint(b.right - 1, b.top + 1));		
	
	if (p) SetHighColor( avg(pc, g255));
	else SetHighColor(g255);
	StrokeLine(BPoint(b.left + 1, b.bottom), BPoint(b.right, b.bottom));
	StrokeLine(BPoint(b.right, b.bottom), BPoint(b.right, b.top + 1));
	
	if (p) SetHighColor( avg(pc, g216));
	else SetHighColor(g216);
	StrokeLine(BPoint(b.left + 2, b.bottom - 1), BPoint(b.right - 1,b.bottom - 1));
	StrokeLine(BPoint(b.right -1, b.bottom - 1), BPoint(b.right - 1, b.top + 2));

	//SetDrawingMode(B_OP_COPY);
	
	SetHighColor(ViewColor());
	
	b.InsetBy(2,2);
	FillRect(b);
}

static inline char int2hex(unsigned int i)
{
	if (i < 10)	return i + '0';
	else return i - 10 + 'a';
}

void ColorView::MouseDown(BPoint p)
{
BMessage msg(B_PASTE);
BBitmap *bmp;
char hex[8];
rgb_color rgb(ViewColor());


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
	//DragMessage(&msg, BRect(20,20,50,50));
	DragMessage(&msg, bmp, BPoint(9,9));
}