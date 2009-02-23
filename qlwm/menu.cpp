/*
*  File      : menu.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Creates menu button and the XKill function
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
#include "toolbar.h"
#include "conf.h"
//#include "moc_menu.cpp"

#define kcursor_width 32
#define kcursor_height 32
uchar menu::kcursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x01,
   0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x08, 0x10, 0x01,
   0x80, 0x10, 0x08, 0x01, 0x80, 0x20, 0x04, 0x01, 0x80, 0x40, 0x02, 0x01,
   0x80, 0x80, 0x01, 0x01, 0x80, 0x80, 0x01, 0x01, 0x80, 0x40, 0x02, 0x01,
   0x80, 0x20, 0x04, 0x01, 0x80, 0x10, 0x08, 0x01, 0x80, 0x08, 0x10, 0x01,
   0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0xff, 0xff, 0x01,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uchar menu::kcursor_mask[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x03,
   0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x1c, 0x38, 0x03,
   0xc0, 0x38, 0x1c, 0x03, 0xc0, 0x70, 0x0e, 0x03, 0xc0, 0xe0, 0x07, 0x03,
   0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xe0, 0x07, 0x03,
   0xc0, 0x70, 0x0e, 0x03, 0xc0, 0x38, 0x1c, 0x03, 0xc0, 0x1c, 0x38, 0x03,
   0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0xff, 0xff, 0x03,
   0xc0, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


menu::menu(QWidget *parent) : QPushButton(parent)
{
	setFixedSize(defaults::tc_height, defaults::tc_height);
	killop = FALSE;
	basemenu = new QMenu(this);
	Q_CHECK_PTR(basemenu);
	basemenu->setFont(defaults::toolbarfont);
	connect(basemenu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
	
	QImage img(qapp::get_cfile("menu.xpm"));

	if(! img.isNull())  // make scaled pixmap
	{
		int wh = defaults::tc_height-4;
		setIcon(QIcon(QPixmap::fromImage(img.scaled(wh, wh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))));
	}
	
	act_quit = new QAction(this);
	act_quit->setData(QVariant(QString("_Quit")));
	act_kill = new QAction(this);
	act_kill->setData(QVariant(QString("_Kill")));
	act_rest = new QAction(this);
	act_rest->setData(QVariant(QString("_Restart")));
}

void menu::readmenu(void)
{
	QString fname = qapp::get_cfile("menuconfig");

	if(fname.isNull())
		return;
		
	QFileInfo fi(fname);

	if(fi.lastModified() == lmtime)
		return;

	lmtime == fi.lastModified();
	basemenu->clear();   
	
	QFile istr(fname);
	if(! istr.open(QIODevice::ReadOnly))
	{
		perror("cannot open menu file");
		return;
	}	

	recurse(basemenu, istr);
	istr.close();
}

void menu::winkill(void)
{
	QBitmap bmapc = QBitmap::fromData(QSize(kcursor_width, kcursor_height), kcursor_bits);
	QBitmap bmapm = QBitmap::fromData(QSize(kcursor_width, kcursor_height), kcursor_mask);
	grabMouse(QCursor(bmapc, bmapm));
	
	killop = TRUE;
}

void menu::recurse(QMenu *cm, QFile &istr)
{
	QString p1,p2,p3;
	
	while(! istr.atEnd())
	{
		QTextStream til(istr.readLine(1024));

		til >> p1;  // command 
		if(p1.isEmpty() || p1 == "#")
			continue;
		
		if(p1 == "End")
			return;

		if(p1 == "Separator")
		{
			cm->addSeparator();
			continue;
		}	

		til >> p2;            // menu label
		
		if(p2.isEmpty())
			continue;

		p3 = til.readLine();  // command line

		if(p2[0] == '"')
		{
			if(p2[p2.length()-1] != '"')
			{
				int i = p3.indexOf('"');
				if(i == -1)
					continue;
				
				p2 += p3.left(i);
				p2 = p2.mid(1, p2.length()-1);
				p3 = p3.right(p3.length()-i-1);
			}
			else p2 = p2.mid(1, p2.length()-2);
		}
		
		if(p1 == "Menu")
		{
			QMenu *nm = new QMenu(p2);
			Q_CHECK_PTR(nm);
			nm->setFont(defaults::toolbarfont);
			cm->addMenu(nm);
			recurse(nm, istr);
			continue;
		}

		if(p1 == "Quit")
		{
			act_quit->setText(p2);
			cm->addAction(act_quit);
			continue;
		}
		
		if(p1 == "Kill")
		{
			act_kill->setText(p2);
			cm->addAction(act_kill);
			continue;
		}	

		if(p1 == "Restart")
		{
			act_rest->setText(p2);
			cm->addAction(act_rest);
			continue;
		}	
		
		if(p1 != "Entry")
			continue;

		p3 = p3.simplified();
		
		if(p3.isEmpty())
			continue;
			
		QAction *act_run = new QAction(p2, cm);
		act_run->setData(QVariant(p3));
		cm->addAction(act_run);
	}
}	

void menu::run_cmd(QByteArray &cmd)  // execute menu item
{
	pid_t pid;

	if((pid = fork()) == -1)
	{
		perror("fork()");
		return;
	}	
	if(pid)
		return;

	execl("/bin/sh", "sh", "-c", cmd.data(), NULL);
	perror("cannot run /bin/sh");
	exit(1); 
}

void menu::run_menu(QAction *act)
{
	QByteArray cmd = act->data().toByteArray();
	
	if(cmd == "_Quit")
		winkill_all();
	else if(cmd == "_Kill")
		winkill();
	else if(cmd == "_Restart")
		qapp::wm_restart();
	else run_cmd(cmd);
}

void menu::start_popup(void)
{	
	Window rw,cw;
	int rx,ry,wx,wy;
	unsigned mr;

	if(qapp::menu_open)
		return;
	
	xwindow *focusclient = qapp::focusclient;
	qapp::menu_open = TRUE;
	
	setDown(TRUE);

	if(! defaults::show_menu || qapp::smode)  // show at mouse position
	{
		XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(), &rw, &cw, &rx, &ry, &wx, &wy, &mr);
		basemenu->exec(QPoint(rx, ry));
	}
	else
	{
		if(! defaults::toolbar_top)   // menu above button
		{
			QPoint p = mapToGlobal(QPoint(0, 0));
			QSize s(basemenu->sizeHint());
			p.setY(p.y()-s.height());
			basemenu->exec(p);
		}
		else
			basemenu->exec(mapToGlobal(QPoint(0, height())));
	}
	qapp::menu_open = FALSE;
	setDown(FALSE);
	
	XSetInputFocus(QX11Info::display(), QX11Info::appRootWindow(), RevertToPointerRoot, CurrentTime);
	if(focusclient != NULL && clients.indexOf(focusclient) != -1)
		focusclient->setchildfocus(CurrentTime);
}

void menu::mousePressEvent(QMouseEvent *event)
{
	if(killop)  //  window kill operation
	{
		xwindow *client;
		
		killop = FALSE;
		releaseMouse();
		
		if(event->button() != Qt::LeftButton)
			return;
		
		if((client = (xwindow *)QApplication::widgetAt(event->globalPos())) == NULL)
			return;
		
		XKillClient(QX11Info::display(), client->client_id());
		XSync(QX11Info::display(), FALSE);
		
		return;
	}
	start_popup();
}


void menu::winkill_all()
{ 
        //HomeDesktop::instance()->getProcbar()->remove_all();
	for(int i=0; i < clients.size(); i++) 
		clients.at(i)->wdestroy();
		
	clients.clear();
	XSync(QX11Info::display(), FALSE);
	exit(0);
}
