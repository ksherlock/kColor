#ifndef __ColorView_h__
#define __ColorView_h__

#include <View.h>

class ColorView: public BView
{
public:

	ColorView(BRect r);
	
virtual void Draw(BRect r);
virtual void MouseDown(BPoint point);

};


#endif
