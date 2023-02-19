/********************************************************************
*
*		Program: cosmotost
*		Developed by: DAHM
*   	Group 1: (D)aniel, (A)iland, (H)uaiyu, (M)ichael
*		Date Code Originated: 2/17/23
*		Base Code Provided by: Professor Gordon Griesel
*
**********************************************************************/

//                 		INCLUDES

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

// 					LOCAL LIBRARIES
#include "fonts.h"	// used for fonts
#include "Box.h"	// base box class
#include "Global.h"	// global vars


using namespace std;

// Global g;
// Box b1;
// extern Box b1;

class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
	void reshape_window(int width, int height);
	void check_resize(XEvent *e);
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

//Function prototypes
void init_opengl(void);
void physics(void);
void render(void);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	init_opengl();
	//Main loop
	int done = 0;
	while (!done) {
		//Process external events.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
		usleep(200);
	}
	return 0;
}

X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		cout << "\n\tcannot connect to X server\n" << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		cout << "\n\tno appropriate visual found\n" << endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	// set initial game state
	g.state = MAINMENU;
}

void X11_wrapper::set_title()
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "c0sm0t0asT");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	//Get a pending event.
	XEvent e;
	XNextEvent(dpy, &e);
	return e;
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
	//window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres) {
		//Window size did change.

		reshape_window(xce.width, xce.height);
		// reshape_window(g.xres, g.yres);	// force game to be 600x800 - test

	}
}
//-----------------------------------------------------------------------------

void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
			//int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.


		}
	}
}

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		switch (key) {
			case XK_1:
				//Key 1 was pressed
				break;
			case XK_Escape:
				//Escape key was pressed
				return 1;
		}
	}
	return 0;
}

// won't have to mess with this much in the project
void init_opengl(void)
{
	//OpenGL initialization
    // make the viewport the size of the whole window
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
    // most likely this will always be set this way in all our programs
    // vertices are drawn and then filled in
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
    // modifies the matrices to be in orthographic mode
    // think of it as a camera looking at things from so far away
    //     that it is in 2 dimensions
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//Set the screen background color
    // makes it almost black
	


    // initialize fonts
    
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

}

void physics()
{

}

void render()
{
	

	if (g.state == MAINMENU) {
		glClearColor(3.0/255, 38.0/255, 18.0/255, 1.0);	// clear it to a bluish
		glClear(GL_COLOR_BUFFER_BIT);	// clear screen
		
		// make text boxes


		// make it so that the box is only visible if the screen is big enough
		
		s_menu_bg.set_color(2, 115, 104);
		glColor3ubv(s_menu_bg.color);
        
		glPushMatrix();
		glTranslatef(g.xres/2.0f, g.yres/2.0f, 0.0f);
		glBegin(GL_QUADS);
			glVertex2f(-s_menu_bg.w, -s_menu_bg.w);
			glVertex2f(-s_menu_bg.w,  s_menu_bg.w);
			glVertex2f( s_menu_bg.w,  s_menu_bg.w);
			glVertex2f( s_menu_bg.w, -s_menu_bg.w);
		glEnd();
		glPopMatrix();

		Rect r;
		r.bot = g.yres/2.0f;
		r.left = g.xres/2.0f;
		r.center = 1;

		ggprint8b(&r, 16, 0x00011F26, "c0sm0t0asT");

		
	} else if (g.state == GAME) {



	} else if (g.state == PAUSE) {


	} else if (g.state == GAMEOVER) {


	}

	

}






