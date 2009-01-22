/* 
*  File      : winlist.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Creates the winlist button
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
#include "toolbar.h"
#include "winlist.h"
#include "xwindow.h"


winlist::winlist(QWidget *parent) : QPushButton(parent)
{
	setFixedSize(defaults::tc_height, defaults::tc_height);
	wmenu = new QMenu(this);
	Q_CHECK_PTR(wmenu);
	connect(wmenu, SIGNAL(hovered(QAction *)), SLOT(highlight_pager(QAction *)));
	wmenu->setFont(defaults::toolbarfont);
	int wh = defaults::tc_height-4;
	QImage oimg(qapp::get_cfile("winlist.xpm"));
	QImage timg(qapp::get_cfile("tiled.xpm"));

	if(! oimg.isNull())  // scale pixmap
		winlisticn.addPixmap(QPixmap::fromImage(oimg.scaled(wh, wh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));

	if(! timg.isNull()) 
		tiledicn.addPixmap(QPixmap::fromImage(timg.scaled(wh, wh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void winlist::set_pixmap(void)
{
	static int showtiled = 0;

	if(qapp::is_tileddesk())
	{
		if(showtiled == 1)
			return;

		setIcon(tiledicn);
		showtiled = 1;
	}
	else
	{
		if(showtiled == 2)
			return;

		setIcon(winlisticn);
		showtiled = 2;
	}	
}

// returns TRUE if client is fully obscured by other toplevel windows

bool winlist::isobscured(xwindow *client, Window *wins, uint nwins)
{
	xwindow *nclient;
	uint cwin;

	for(cwin=0; cwin < nwins; cwin++)  // find current window
		if(wins[cwin] == client->winId())
			break;

	if(cwin >= nwins)
		return FALSE;

	QRegion sr(client->frameGeometry());
	sr -= QRegion(1,1,1,1);
	
	while(++cwin < nwins)
	{
		if((nclient = qapp::pwindows[wins[cwin]]) == NULL || ! nclient->isVisible())
			continue;
		
		sr -= QRegion(nclient->frameGeometry());

		if(sr.isEmpty())
			return TRUE;
	}
	return FALSE;
}

// returns TRUE if client is below tiled windows

bool winlist::isbottom(xwindow *client, Window *wins, uint nwins)
{
	xwindow *nclient;
	uint cwin;

	for(cwin=0; cwin < nwins; cwin++) 
		if(wins[cwin] == client->winId())
			break;

	if(cwin >= nwins)
		return FALSE;

	while(++cwin < nwins)
	{
		if((nclient = qapp::pwindows[wins[cwin]]) == NULL || ! nclient->isVisible() || ! qapp::is_curdesk(nclient))
			continue;

		if(nclient->is_tiled())
			return TRUE;
	}
	return FALSE;
}	

void winlist::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton)
		setDown(FALSE);
}

void winlist::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton)  // hidden to foreground
	{
		setDown(TRUE);
		hidden_win();
	}
	else if(event->button() == Qt::LeftButton)
		start_popup();
}

void winlist::hidden_win(void)
{
	Window w1,w2,*wins;
	uint nwins;
	int i=0;

	if(XQueryTree(QX11Info::display(), QX11Info::appRootWindow(), &w1, &w2, &wins, &nwins) == 0 || ! nwins)
		return;

	xwindow *win;
	int dwidth = QApplication::desktop()->width();

	foreach(win, clients)
	{
		if(win->isVisible() && ! (win->get_pflags() & qapp::StayOnBottom) && win->x() <= dwidth && win->x() >= 0 && isobscured(win, wins, nwins))
		{
			win->raise();
			XFree(wins);
			return;
		}	
	}

	if(qapp::is_tileddesk())
	{
		foreach(win, clients)
		{
			win = clients.at(i);
			if(win->isVisible() && ! (win->get_pflags() & qapp::StayOnBottom) && win->x() <= dwidth && win->x() >= 0 && ! win->is_tiled() && isbottom(win, wins, nwins))
			{
				win->raise();
				XFree(wins);
				return;
			}
		}
	}
	XFree(wins);
}


void winlist::highlight_pager(QAction *act)
{
	if(qapp::smode)
		return;
        HomeDesktop *tb = HomeDesktop::instance();
        tb->getPager()->draw_pager();
        tb->getPager()->add(clients.at(act->data().toInt()), TRUE);
}

void winlist::start_popup(void)
{
	if(qapp::menu_open)
		return;

	xwindow *focusclient = qapp::focusclient;
	qapp::menu_open = TRUE;
	
	setDown(TRUE);
	popup_list();

        HomeDesktop::instance()->getPager()->draw_pager();
	qapp::menu_open = FALSE;

	XSetInputFocus(QX11Info::display(), QX11Info::appRootWindow(), RevertToPointerRoot, CurrentTime);
	if(focusclient != NULL && clients.indexOf(focusclient) != -1)
		focusclient->setchildfocus(CurrentTime);
}

void winlist::popup_list(void)
{
	xwindow *win;
	QString wname;
	Window rw,cw;
	int rx,ry,wx,wy;
	unsigned mr;
	int i;

	for(i=0; i < clients.size(); i++)
	{
		win = clients.at(i);
		if(win->iswithdrawn() || ((win->get_pflags() & qapp::WindowListSkip) && ! win->hidden_win()))
			continue;

		wname = "";
		QTextStream wnm(&wname);
		
		if(! win->isVisible())
			wnm << '<';
			
		wnm << win->ccaption().left(100);

		if(! win->isVisible())
			wnm << '>';

	 	if(win->get_tnumber())
			wnm << '<' << win->get_tnumber() << '>';
		
		if(! win->getmachine().isNull())
			wnm << " (" << win->getmachine().left(20) << ')';
			
		QAction *act_run = new QAction(wname, wmenu);
		act_run->setData(QVariant(i));
		connect(act_run, SIGNAL(triggered()), clients.at(i), SLOT(focus_mouse_wlist()));
		wmenu->addAction(act_run);
	}

	if(! defaults::show_winlist || qapp::smode)  // show at mouse position
	{
		XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(), &rw, &cw, &rx, &ry, &wx, &wy, &mr);
		wmenu->exec(QPoint(rx, ry));
	}
	else
	{
		if(! defaults::toolbar_top)   // menu above button
		{
			QPoint p = mapToGlobal(QPoint(0, 0));
			QSize s(wmenu->sizeHint());
			p.setY(p.y()-s.height());
			wmenu->exec(p);
		}
		else
			wmenu->exec(mapToGlobal(QPoint(0, height())));
	}		
		
	wmenu->clear();
	setDown(FALSE);
}
