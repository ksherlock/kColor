#ifndef __ColorWell_h__
#define __ColorWell_h__

#include <View.h>

class ColorWell: public BView
{
public:
	ColorWell(BRect);

void SetColor(rgb_color);
void SetColor(uint8, uint8, uint8);
rgb_color GetColor(void) const;
virtual void Draw(BRect);
virtual void MouseDown(BPoint);
private:
	rgb_color color;
};

#endif