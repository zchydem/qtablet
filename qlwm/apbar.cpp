/* 
*  File      : apbar.cpp 
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Manages the applets on the right side of the toolbar
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
#include "toolbar.h"
#include "apbar.h"

apbar::apbar(QWidget *parent) : QWidget(parent)
{
	setFixedHeight(defaults::tc_height);
	setFixedWidth(1);

    	// overwrite Qt-defaults because we need SubstructureNotifyMask
	
	XSelectInput(QX11Info::display(), winId(),
  		 PointerMotionMask |
		 EnterWindowMask | LeaveWindowMask |
  		 ExposureMask |
		 StructureNotifyMask |
		 SubstructureRedirectMask |
		 SubstructureNotifyMask);
}

void apbar::paletteChange(const QPalette &)  // propagate property event to clients
{
	XPropertyEvent xe;

	for(int i=0; i < ap.size(); i++)
	{
		xe.type = PropertyNotify;
		xe.window = QX11Info::appRootWindow();
		xe.state = PropertyNewValue;
		xe.atom = qapp::wm_resource_manager;
		XSendEvent(QX11Info::display(), ap.at(i)->w, True, PropertyChangeMask, (XEvent *)&xe);
	}
}

void apbar::place_clients(void)
{
	WINDOW *cur;
	int twidth=2;  // total width
	int i;

	for(i=0; i < ap.size(); i++)
		twidth += ap.at(i)->width+4;

	twidth -= 2;
	setFixedWidth(twidth);

	if(! twidth)
		return;
		
	int cx = 2;
	foreach(cur, ap)
	{
		XMoveWindow(QX11Info::display(), cur->w, cx, 0);
		
		XWindowAttributes attr;
		if(XGetWindowAttributes(QX11Info::display(), cur->w, &attr))
		{
			XMapWindow(QX11Info::display(), cur->w);
			XRaiseWindow(QX11Info::display(), cur->w);
		}	
		cx += cur->width+4;
	}
}

void apbar::release_all(void)
{
	WINDOW *cur;
	
	foreach(cur, ap)
	{
		XReparentWindow(QX11Info::display(), cur->w, QX11Info::appRootWindow(), 0, 0);
		delete cur;
	}	
	ap.clear();
}		

bool apbar::add(Window w, int number, QString &name)
{
	WINDOW *cur;
	int i,i2=2,pnum=1;
        HomeDesktop * home = HomeDesktop::instance();
	foreach(cur, ap)
	{
		if(cur->name == name)
			return FALSE;   // already exists

		if(number > cur->number)
			pnum = i2;

		i2++;	
	} 

	XWindowAttributes attr;
	if(! XGetWindowAttributes(QX11Info::display(), w, &attr))
		return TRUE;

	int twidth=2;  // total width
	for(i=0; i < ap.size(); i++)
		twidth += ap.at(i)->width+4;

        if(twidth+attr.width+100 > 90 /*home->getPager()->width()*/)
	{
		logmsg << "WM: No space left on toolbar\n";
		return FALSE;
	}

	WINDOW *client = new WINDOW;

	client->w = w;
	client->name = name;
	client->width = attr.width;
	client->number = number;
	ap.insert(pnum-1, client);
	
	XSetWindowBorderWidth(QX11Info::display(), w, 0);
	XResizeWindow(QX11Info::display(), w, attr.width, defaults::tc_height);
	XReparentWindow(QX11Info::display(), w, winId(), 0, 0);

	place_clients();

	return TRUE;
}

bool apbar::client_exists(Window w)
{
	for(int i=0; i < ap.size(); i++)
	{
		if(ap.at(i)->w == w)
			return TRUE;
	}
	return FALSE;
}

bool apbar::remove(Window w)
{
	WINDOW *cur;
	
	for(int i=0; i < ap.size(); i++)
	{
		cur = ap.at(i);
		if(cur->w == w)
		{
			ap.removeAt(i);
			delete cur;
			place_clients();
			return TRUE;
		}
	}
	return FALSE;
}

void apbar::remove(void)
{
	WINDOW *cur;
	bool place = FALSE;
	Atom type;
	int format;
	unsigned long nitems=0;
	unsigned long extra=0;
	unsigned char *data=NULL;

	for(int i=0; i < ap.size(); i++)
	{
		cur = ap.at(i);
		if(qapp::apclients[cur->name] == 0)
		{
			XGetWindowProperty(QX11Info::display(), cur->w, qapp::kde_net_wm_system_tray_window_for, 0, 1L, FALSE, XA_WINDOW, &type, &format, &nitems, &extra, &data);

			if(data != NULL)
			{
				XFree(data);
				data = NULL;
			}	

			if(nitems)
			{
				nitems = 0;
				continue;
			}	
			
			ap.removeAt(i--);
			XKillClient(QX11Info::display(), cur->w);
			delete cur;
			place = TRUE;
		}
	}
	if(place)
		place_clients();
}
