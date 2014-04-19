#ifndef __window_h__
#define __window_h__

#include <Window.h>

class ColorControl;
class ColorView;
class BView;
class BTextControl;
class BPopUpMenu;
class BMenuItem;
class ColorWell;

class window: public BWindow
{
public:

	window();

virtual void MessageReceived(BMessage *);
virtual void Quit(void);

private:

void LoadPrefs();

BView *back;

ColorControl *r;
ColorControl *g;
ColorControl *b;

BTextControl *rtxt;
BTextControl *gtxt;
BTextControl *btxt;

ColorView *cv;
BPopUpMenu *menu;
BMenuItem *modes[3];
uint32 mode;

ColorWell *cw[13];

rgb_color color;
};



#endif