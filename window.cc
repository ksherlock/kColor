#include "window.h"
#include <Application.h>
#include <View.h>

#include "ColorControl.h"
#include "ColorView.h"
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <TextControl.h>
#include <String.h>
#include <TextView.h>
#include <stdio.h>
#include "ColorWell.h"

class myView: public BView
{
public:

	myView(BRect r, const char *cp, uint32 d, uint32 res);
virtual void MouseDown(BPoint);	

};

myView::myView(BRect r, const char *cp, uint32 d, uint32 res):
	BView(r, cp, d, res)
{
}
void myView::MouseDown(BPoint p)
{
BPoint p2;
uint32 b;
	
	GetMouse(&p2, &b);
	if (Window() && (b & B_SECONDARY_MOUSE_BUTTON))
	{
		BMessage msg('menu');
		ConvertToScreen(&p);
		msg.AddPoint("point", p);
		Window()->PostMessage(&msg);
	}
}

window::window():
	BWindow (BRect(200,200,600,330), "kColor",
		B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
		mode(0xffff)
{
//BView *back;
BMenuItem *item;

	back = new myView(Bounds(), "", B_WILL_DRAW, B_FOLLOW_ALL);
	back->SetViewColor(216,216,216);
	AddChild(back);

	rtxt = new BTextControl(BRect(10, 11, 40, 31), "", "", "0", new BMessage('text'));
	rtxt->SetDivider(0);
	back->AddChild(rtxt);

	gtxt = new BTextControl(BRect(10, 36, 40, 56), "", "", "0", new BMessage('text'));
	gtxt->SetDivider(0);
	back->AddChild(gtxt);

	btxt = new BTextControl(BRect(10, 61, 40, 71), "", "", "0", new BMessage('text'));
	btxt->SetDivider(0);
	back->AddChild(btxt);

	r = new ColorControl(BRect(10 + 40, 10, 270 + 40, 30), new BMessage('_clr'));
	r->SetColor(0);
	back->AddChild(r);
	
	g = new ColorControl(BRect(10 + 40, 35, 270 + 40, 55), new BMessage('_clr'));
	g->SetColor(1);
	back->AddChild(g);
	
	b = new ColorControl(BRect(10 + 40, 60, 270 + 40, 70), new BMessage('_clr'));
	b->SetColor(2);
	back->AddChild(b);
	
	cv = new ColorView(BRect(280 + 40, 10, 350 + 40, 80));
	cv->SetViewColor(0,0,0);
	back->AddChild(cv);
	
	menu = new BPopUpMenu("", false, false);

	menu->AddItem(modes[0] = new BMenuItem("Float Mode", new BMessage('mode')));	
	menu->AddItem(modes[1] = new BMenuItem("Hex Mode", new BMessage('mode')));
	menu->AddItem(modes[2] = new BMenuItem("Integer Mode", new BMessage('mode')));
	modes[2]->SetMarked(true);
	{
		BMessage msg('mode');
		msg.AddInt32("index", 2);
		PostMessage(&msg); // fake an invocation
	}
	menu->AddItem(new BSeparatorItem());
	
	for (uint32 i = 0; i < 3; i++)
		modes[i]->SetTarget(this);
	
	menu->AddItem(item = new BMenuItem("About...", new BMessage(B_ABOUT_REQUESTED)));
	item->SetTarget(be_app);
	menu->AddItem(item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
	item->SetTarget(be_app);
	
	AddShortcut(B_LEFT_ARROW, B_COMMAND_KEY, new BMessage('left'));
	AddShortcut(B_RIGHT_ARROW, B_COMMAND_KEY, new BMessage('rght'));
	
	for (uint32 i = 0; i < 13; i++)
	{
		cw[i] = new ColorWell(BRect( 10 + i * 30, 100, 10 + 20 + i * 30 , 120));
		back->AddChild(cw[i]);
		cw[i]->SetViewColor(B_TRANSPARENT_COLOR);
		cw[i]->SetColor(0,0,0);
	}
	LoadPrefs();
}

static inline bool operator != (rgb_color r1, rgb_color r2)
{
	if (r1.red == r2.red
		&& r1.green == r2.green
		&& r1.blue == r2.blue
		/*&& r1.alpha == r2.alpha*/) return false;
	else return true;

}
static inline char int2hex(unsigned int i)
{
	if (i < 10)	return i + '0';
	else return i - 10 + 'A';
}


static uint32 getval(const char *cp, uint32 mode)
{
int32 ret = 0;

	switch(mode)
	{
	case 0: // float
		{
			float f = 0;
			sscanf(cp, "%f", &f);
			ret = (int32)(f * 255);
		}
		break;
	case 1: //hex
		{
			sscanf(cp, "%x", &ret);
		}
		break;
	case 2: // integer mode
		{
			sscanf(cp, "%d", &ret);
		}
		break;
	}
	
	if (ret > 255) ret = 255;
	if (ret < 0) ret = 0;

	return ret;
}

void window::MessageReceived(BMessage *msg)
{
	//msg->PrintToStream();
	switch(msg->what)
	{
	case 'text':
		{
			BTextControl *tc;
			const char *cp;
			//msg->PrintToStream();
			if (msg->FindPointer("source", (void **)&tc) == B_OK)
			{
				if (tc == rtxt)
				{
					cp = tc->Text();
					r->SetValue(getval(cp, mode));
					r->Invoke();
				}
				else if (tc == gtxt)
				{
					cp = tc->Text();
					g->SetValue(getval(cp, mode));
					g->Invoke();
				}
				else if (tc == btxt)
				{
					cp = tc->Text();
					b->SetValue(getval(cp, mode));
					b->Invoke();
				}
			}
		}
		break;
	
	case 'mode':
		{
			//msg->PrintToStream();	
			int32 i;
			if (msg->FindInt32("index", &i) == B_OK)
			{
				if (i == mode) break;
				if (mode < 3) modes[mode]->SetMarked(false);
				mode = i;
				modes[mode]->SetMarked(true);
				// update the values...
				
				switch (mode)
				{
				case 0: //float
					{
						BTextView *tv;
						BString str;
						
						tv = rtxt->TextView();
						tv->SetMaxBytes(4);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for	(uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						tv->AllowChar('.');
						
						tv = gtxt->TextView();
						tv->SetMaxBytes(4);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for	(uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						tv->AllowChar('.');
						
						tv = btxt->TextView();
						tv->SetMaxBytes(4);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for	(uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						tv->AllowChar('.');
						
						str = "";
						str << (float)(float)color.red/(float)255;
						rtxt->SetText(str.String());
						
						str = "";
						str << (float)(float)color.green/(float)255;
						gtxt->SetText(str.String());
						
						str = "";
						str << (float)(float)color.blue/(float)255;
						btxt->SetText(str.String());
					}
					break;
				case 1: //hex
					{
						BTextView *tv;
						BString str;
						
						tv = rtxt->TextView();
						tv->SetMaxBytes(2);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for (uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'a'; x < 'f' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'A'; x < 'F' + 1; x++)
							tv->AllowChar(x);
							
						tv = gtxt->TextView();
						tv->SetMaxBytes(2);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for (uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'a'; x < 'f' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'A'; x < 'F' + 1; x++)
							tv->AllowChar(x);
							
						tv = btxt->TextView();
						tv->SetMaxBytes(2);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(i);
						for (uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'a'; x < 'f' + 1; x++)
							tv->AllowChar(x);
						for (uint32 x = 'A'; x < 'F' + 1; x++)
							tv->AllowChar(x);
				
						str = "";
						str += int2hex(color.red >> 4);
						str += int2hex(color.red & 0x0f);
						rtxt->SetText(str.String());
						
						str = "";
						str += int2hex(color.green >> 4);
						str += int2hex(color.green & 0x0f);
						gtxt->SetText(str.String());
						
						str = "";
						str += int2hex(color.blue >> 4);
						str += int2hex(color.blue & 0x0f);
						btxt->SetText(str.String());
					
					}
					break;
				case 2: // integer
					{
						BTextView *tv;
						BString str;
						
						tv = rtxt->TextView();
						tv->SetMaxBytes(3);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for(uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
						
						tv = gtxt->TextView();
						tv->SetMaxBytes(3);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for (uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);	
							
						tv = btxt->TextView();
						tv->SetMaxBytes(3);
						for (uint32 x = 0; x < 255; x++)
							tv->DisallowChar(x);
						for (uint32 x = '0'; x < '9' + 1; x++)
							tv->AllowChar(x);
							
						str = "";
						str << (int32)color.red;
						rtxt->SetText(str.String());
						
						str = "";
						str << (int32)color.green;
						gtxt->SetText(str.String());
						
						str = "";
						str << (int32)color.blue;
						btxt->SetText(str.String());
					}
					break;
				}
			}
		}
		break;
			
	case 'left':
		{
			ColorControl *c = NULL;
			if (r->IsFocus()) c = r;
			else if (g->IsFocus()) c = g;
			else if (b->IsFocus()) c = b;
			
			if (c)
			{
				uint32 v = c->Value();
				if (v == 0) break;
				if (v > 10) v -= 10;
				else v = 0;
				c->SetValue(v);
				c->Invoke();
			}
		}
		break;
	
	case 'rght':
		{
			ColorControl *c = NULL;
			if (r->IsFocus()) c = r;
			else if (g->IsFocus()) c = g;
			else if (b->IsFocus()) c = b;
			
			if (c)
			{
				uint32 v = c->Value();
				if (v == 255) break;
				if (v < 245) v += 10;
				else v = 255;
				c->SetValue(v);
				c->Draw(c->Bounds());
				c->Invoke();
			}		
		}
		break;
		
	case B_ABOUT_REQUESTED:
		be_app->PostMessage(B_ABOUT_REQUESTED);
		break;
	case B_QUIT_REQUESTED:
		Quit();
		break;
	case 'menu':
		{
		BPoint p;
			if (msg->FindPoint("point", &p) == B_OK)
			{
				menu->Go(p, true, false, true);
			}
		}
		break;	

	case B_PASTE:
		{
			rgb_color *rgb;
			ssize_t size;
			BPoint where;
			//msg->PrintToStream();
			if (msg->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void **)&rgb,
				&size) == B_OK && msg->FindPoint("_drop_point_", &where) == B_OK)
			{
				where = ConvertFromScreen(where); // get local coordinates
				if (cv->Frame().Contains(where)
					|| r->Frame().Contains(where)
					|| g->Frame().Contains(where)
					|| b->Frame().Contains(where))
				{
					if (color != *rgb)
					{
						color = *rgb;
						cv->SetViewColor(color);
						cv->Draw(cv->Bounds());
						r->SetMask(color);
						g->SetMask(color);
						b->SetMask(color);
						if (r->Value() != color.red)
							r->SetValue(color.red);	
						if (g->Value() != color.green)
							g->SetValue(color.green);
						if (b->Value() != color.blue)
							b->SetValue(color.blue);
						r->Draw(r->Bounds());
						g->Draw(g->Bounds());
						b->Draw(b->Bounds());
						// set the text...
						r->Invoke();
						g->Invoke();
						b->Invoke();
					}
					break;
				}
				bool done = false;
				for (uint32 i = 0; i < 13; i++)
				{
					if (cw[i]->Frame().Contains(where))
					{
						done = true;
						cw[i]->SetColor(*rgb);
						cw[i]->Draw(cw[i]->Bounds());
					}
				}
				if (done) break;
				
				back->SetViewColor(*rgb);
				back->Invalidate();
				cv->Draw(cv->Bounds());
				r->Draw(r->Bounds());
				g->Draw(g->Bounds());
				b->Draw(b->Bounds());
			}
		}
		break;
		
	case '_clr':
		{
		int32 val = 0;
		BString str;
		void *ptr = NULL;
			msg->FindInt32("be:value", &val);
			msg->FindPointer("source", &ptr);
			
			switch (mode)
			{
			case 0: //float
				str << (float)(float)val/(float)255;
				break;
			case 1: //hex
				str += int2hex(val >> 4); str += int2hex(val & 0x0f);
				break;
			case 2: // integer
				str << val;
				break;
			}
			
			if (ptr == r)
			{
				color.red = val;
				g->SetMask(color);
				b->SetMask(color);
				g->Draw(g->Bounds());
				b->Draw(b->Bounds());
				rtxt->SetText(str.String());
			}
			if (ptr == g)
			{
				color.green = val;
				r->SetMask(color);
				b->SetMask(color);
				r->Draw(r->Bounds());
				b->Draw(b->Bounds());
				gtxt->SetText(str.String());
			}
			if (ptr == b)
			{
				color.blue = val;
				r->SetMask(color);
				g->SetMask(color);
				r->Draw(r->Bounds());
				g->Draw(g->Bounds());
				btxt->SetText(str.String());	
			}
			cv->SetViewColor(color);
			cv->Draw(cv->Bounds());
		}
		break;

		
	default: BWindow::MessageReceived(msg);
	}
}
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
void window::Quit(void)
{
BPath path;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK)
	{
		path.Append("kColor", true);

		BFile file;
		if (file.SetTo(path.Path(), B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE) == B_OK)
		{
			BString str;
			BMessage msg(1);
			rgb_color rgb;
			//add the color wells...
			for (uint32 i = 0; i < 13; i++)
			{
				str ="";
				str <<(int32) i;
				rgb = cw[i]->GetColor();
				msg.AddData(str.String(), B_RGB_COLOR_TYPE, &rgb, sizeof(rgb_color));
			}
			
			//add the color...
			msg.AddData("color", B_RGB_COLOR_TYPE, &color, sizeof(rgb_color));
			
			//add the background color
			rgb = back->ViewColor();
			msg.AddData("back", B_RGB_COLOR_TYPE, &rgb, sizeof(rgb_color));
			
			//add the mode...
			msg.AddInt32("mode", mode);
			
			//msg.PrintToStream();
			msg.Flatten(&file);
		}

	}
	be_app->PostMessage(B_QUIT_REQUESTED);
	BWindow::Quit();
}

void window::LoadPrefs(void)
{
BPath path;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK)
	{
		path.Append("kColor", true);

		BFile file;
		if (file.SetTo(path.Path(), B_READ_ONLY) == B_OK)
		{
			BMessage msg(1);
			if (msg.Unflatten(&file) == B_OK)
			{
				//msg.PrintToStream();
				BString str;
				rgb_color *rgb;
				ssize_t st;
				for (uint32 i = 0; i < 13; i++)
				{
					str = "";
					str << (int32)i;
					if (msg.FindData(str.String(), B_RGB_COLOR_TYPE,
						(const void **)&rgb, &st) == B_OK)
					{
						cw[i]->SetColor(*rgb);
						//cw[i]->Draw(); // hasn't drawn yet, anyway...
					}
				}
				uint32 m;
				if (msg.FindInt32("mode", (int32 *)&m) == B_OK)
				{
					if ((m != mode) && (m < 3))
					{
						BMessage mm('mode');
						mm.AddInt32("index", (int32)m);
						PostMessage(&mm);
					}
				}
				
				if (msg.FindData("back", B_RGB_COLOR_TYPE,
					(const void **)&rgb, &st) == B_OK)
				{
					back->SetViewColor(*rgb);
				}
				
				if (msg.FindData("color", B_RGB_COLOR_TYPE, (const void **)&rgb,
					&st) == B_OK)
				{
					color = *rgb;
					r->SetValue(color.red);
					g->SetValue(color.green);
					b->SetValue(color.blue);
					r->Invoke();
					g->Invoke();
					b->Invoke();
				}
			}
		}
	}
}