#ifndef __ColorControl_h__
#define __ColorControl_h__

#include <Control.h>

class ColorControl: public BControl
{
public:

	ColorControl(BRect, BMessage *);
	
virtual void MouseDown(BPoint);
//virtual void MouseMoved(BPoint, uint32, BMessage *);
//virtual void MouseUp(BPoint);
virtual void Draw(BRect);
virtual void KeyDown(const char *, int32);

void SetMask(rgb_color);
void SetColor(uint32); // 0, 1, 2


private:

static int32 __mouse(void *data);

rgb_color mask;
uint32 color;

uint32 old_val;

};

#endif