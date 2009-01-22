/*
*  File      : rubber.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Creates the rubberbanding for move and resize
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
#include "rubber.h"

rubber::rubber(QWidget *parent) : QLabel(parent)  // move
{
	mpos = TRUE;
	init();
}

rubber::rubber(int basew, int baseh, int incw, int inch, QWidget *parent) : QLabel(parent)  // resize
{
	base_w = basew;
	base_h = baseh;
	inc_w = incw;
	inc_h = inch;
	mpos = FALSE;
	init();
}

void rubber::init(void)
{
	drawn = FALSE;

	setFrameStyle(QFrame::Box|QFrame::Plain);
	setLineWidth(1);
	setGeometry(QApplication::desktop()->width()/2-50, QApplication::desktop()->height()/2, 100, defaults::tb_height);

	XGCValues gv;
	
	gv.function = GXxor;
	gv.subwindow_mode = IncludeInferiors;
	gv.foreground = WhitePixel(QX11Info::display(), QX11Info::appScreen());
	gv.background = BlackPixel(QX11Info::display(), QX11Info::appScreen());
	gv.line_width = 1;
	gc_thin = XCreateGC(QX11Info::display(), QX11Info::appRootWindow(), 
	GCForeground|GCBackground|GCFunction|GCLineWidth|GCSubwindowMode, &gv);
	
	gv.line_width = 2;
	gc_thick = XCreateGC(QX11Info::display(), QX11Info::appRootWindow(),
	GCForeground|GCBackground|GCFunction|GCLineWidth|GCSubwindowMode, &gv);
	setFont(defaults::toolbarfont);
}

void rubber::drawframe(void)
{
	XDrawRectangle(QX11Info::display(), QX11Info::appRootWindow(), gc_thick, px, py, pw, ph);
	XDrawSegments(QX11Info::display(), QX11Info::appRootWindow(), gc_thin, segs, 4);
}

void rubber::draw(int x, int y, int w, int h)
{
	if(drawn && old_x == x && old_y == y && old_w == w && old_h == h)  // no changes
		return;

	old_x = x; old_y = y;
	old_w = w; old_h = h;
		
	if(drawn)  // remove old frame
		drawframe();

	QString txt;
	if(mpos)
		txt.sprintf(" x:%d y:%d ", x+(qapp::adesk*QApplication::desktop()->width()), y);
	else
	{
		if(inc_w > 1 || inc_h > 1)
		{
			int i = 1;
			int j = 1;
			
			if(inc_w > 1)
				i = (w-base_w)/inc_w;

			if(inc_h > 1)
				j = (h-base_h)/inc_h;
			
			txt.sprintf(" c:%d r:%d ", i, j);
		}
		else
			txt.sprintf(" w:%d h:%d ", w, h);
	}

	if(! drawn)
	{
		XGrabServer(QX11Info::display());
		show();
		XSync(QX11Info::display(), FALSE);
		qapp::mrb = TRUE;
		drawn = TRUE;
	}	
	setText(txt);
	update();

	// draw frame
		
	int xl = w/3;
	int yl = h/3;
	segs[0].x1 = x; segs[0].x2 = x+w; segs[0].y1 = segs[0].y2 = y+yl;
	segs[1].x1 = x; segs[1].x2 = x+w; segs[1].y1 = segs[1].y2 = y+2*yl;
	segs[2].x1 = segs[2].x2 = x+xl; segs[2].y1 = y; segs[2].y2 = y+h;
	segs[3].x1 = segs[3].x2 = x+2*xl; segs[3].y1 = y; segs[3].y2 = y+h;
	px = x; py = y; pw = w; ph = h;

	drawframe();
}

rubber::~rubber(void)
{
	if(drawn)
		drawframe();

	qapp::mrb = FALSE;

	XUngrabServer(QX11Info::display());	
	XFreeGC(QX11Info::display(), gc_thin);
	XFreeGC(QX11Info::display(), gc_thick);
}
