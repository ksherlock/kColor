#include <Application.h>
#include "window.h"
#include <Alert.h>
class App: public BApplication
{
public:

	App();
	
virtual void AboutRequested(void);

};

App::App():
	BApplication("application/x-vnd.kws-kColor")
{
window *w = new window;
	w->Show();
}

void App::AboutRequested(void)
{
	BAlert *b;
	
	b = new BAlert("", "kColor 1.0\n"
						"Copyright 2000 Kelvin W Sherlock\n\n"
						"www.xbar.org",
						"Ok");
	b->Go(NULL);

}

int main(int argc, char **argv)
{
	App app;
	app.Run();

	return 0;
}
