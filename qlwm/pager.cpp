/*
*   File      : pager.cpp
*   Written by: alinden@gmx.de
*   Copyright : GPL
*
*   Creates the pager
*/

#include "defs.h"
#include "qapp.h"
#include "toolbar.h"
#include "pager.h"
#include "xwindow.h"
#include "defaults.h"

pager::pager(QWidget *parent) : QLabel(parent)
{
	dwidth = QApplication::desktop()->width();
	dheight = QApplication::desktop()->height();

	setFixedSize((int)((defaults::vdesks*defaults::pager_height*(float)dwidth)/(float)dheight)+2, defaults::pager_height);
	setFrameStyle(QFrame::Panel|QFrame::Sunken);
	setLineWidth(1);

	vw = (width()-2)/defaults::vdesks;
	vh = defaults::pager_height-2;
	backpix = new QPixmap(width()-2, vh);
	draw_pager();
}

void pager::draw_pager(void)   // redraw pager and all clients
{
#ifdef DEBUGMSG
	logmsg << "draw pager\n";
#endif	

	backpix->fill(this, 0, 0);
	QPainter painter(backpix);  // painter on background drawable
	
	// fill active desk
	painter.fillRect(qapp::adesk*vw, 0, vw, vh, defaults::pager_active);

	int pos;
	for(int i=1; i < defaults::vdesks; i++)   // draw separator lines
	{
		pos = i*vw;
		painter.drawLine(pos, 0, pos, vh);
	}

	Window w1,w2,*wins;
	uint nwins,i;
	QRect crect;	
	xwindow *client;
	
	if(XQueryTree(QX11Info::display(), QX11Info::appRootWindow(), &w1, &w2, &wins, &nwins) == 0)
		return;
	
	for(i = 0; i < nwins; i++)
	{
		// Qt widgets get their unmap event after the pager is drawn, that's why
		// we check for the unmapped flag too
		
		if((client = qapp::pwindows[wins[i]]) == NULL || ! client->isVisible() || client->is_unmapped())
			continue;

		// adjust for virtual desktop
		crect.setRect(client->x()+(qapp::adesk*dwidth), client->y(), client->width(), client->height());
			
		// draw client window with color set for current or other desktops
		draw_window(&painter, crect);
	}
	XFree(wins);
	setPixmap(*backpix);
}

void pager::draw_window(QPainter *painter, QRect &crect, bool hlight)
{
	QColor fcol = (crect.x()/dwidth == qapp::adesk || crect.right()/dwidth == qapp::adesk)?(defaults::pager_visible):(defaults::pager_window);

	if(hlight)
		fcol.setRgb(~fcol.red() & 0xff, ~fcol.green() & 0xff, ~fcol.blue() & 0xff);

	float tdw = (float)dwidth*defaults::vdesks;
	float tvw = width()-2;
	
	int wx = (int)((crect.x()*tvw)/tdw);
	int wy = (int)((crect.y()*vh)/dheight);
	int ww = (int)((crect.width()*tvw)/tdw);
	int wh = (int)((crect.height()*vh)/dheight);

	if(ww < 2)
		ww = 2;
	
	if(wh < 2)
		wh = 2;
	
	if(ww > 4 && wh > 4)
	{
		painter->fillRect(wx+1, wy+1, ww-1, wh-1, fcol);
		painter->drawRect(wx, wy, ww, wh);
	}
	else
		painter->fillRect(wx, wy, ww, wh, fcol);
}

void pager::add(xwindow *client, bool hlight)   // draw client
{
	QPainter painter(backpix);
	QRect crect;

	// adjust for virtual desktop
	crect.setRect(client->x()+(qapp::adesk*dwidth), client->y(), client->width(), client->height());
	draw_window(&painter, crect, hlight);
	setPixmap(*backpix);
}

void pager::change_desk(int desk)  // change virtual desktop
{
	xwindow *client;
	
	if(desk == qapp::adesk || desk < 0 || desk >= defaults::vdesks)
		return;

	int dx = (qapp::adesk-desk)*dwidth;
	XGrabServer(QX11Info::display());
	
	bool curdesk;
	bool get_event = FALSE;
	foreach(client, clients)
	{
		curdesk = client->x() <= dwidth && client->x() >= 0;

		if(! (client->get_pflags() & qapp::Sticky) && (client->isVisible() || ! curdesk))
		{
			client->move(client->x()+dx, client->y());
			client->repaint();
			get_event = TRUE;
		}
	}

	qapp::adesk = desk;
	XUngrabServer(QX11Info::display());

	if(defaults::sttiled[desk])
	{
		qapp::toggle_tiled();
		defaults::sttiled[desk] = FALSE;
	}
        HomeDesktop::instance()->getWinlist()->set_pixmap();
	
	if(! get_event)
		draw_pager();
}

void pager::mousePressEvent(QMouseEvent *event)
{
	change_desk(event->pos().x()/vw);
}

void pager::paletteChange(const QPalette &)
{
	draw_pager();
}
