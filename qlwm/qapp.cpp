/*
*  File      : qapp.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Subclassing for QApplication, contains the X11 event
*  filter and some static functions that are common to all
*  classes
*/

#include "defs.h"
#include "conf.h"
#include "defaults.h"
#include "qapp.h"
#include <stdio.h>

#include <QtDebug>

QList <xwindow*> clients;     // client list

HomeDesktop *qapp::home;                          // Toolbar
QPixmap *qapp::leftwinpix;                  // window button pixmap
QPixmap *qapp::rightwinpix;                 // window button pixmap
QHash <Window, xwindow*> qapp::cwindows;    // client window lookup table
QHash <Window, xwindow*> qapp::pwindows;    // parent window lookup table
int qapp::next_x = 10;                      // autoplacement position counter
int qapp::next_y = defaults::tb_height+1;   // autoplacement position counter
int qapp::adesk;                            // current desktop
Atom qapp::wm_protocols;
Atom qapp::wm_delete;
Atom qapp::wm_change_state;
Atom qapp::wm_state;
Atom qapp::wm_take_focus;
Atom qapp::wm_resource_manager;
Atom qapp::wm_colormaps;
Atom qapp::kde_net_wm_system_tray_window_for;
Atom qapp::net_wm_name;
Atom qapp::net_wm_icon_name;
Atom qapp::net_supported;
Atom qapp::net_wm_window_opacity;
bool qapp::smode;
QPalette *qapp::ipal;                       // window inactive palette
QPalette *qapp::upal;                       // window urgent palette
bool qapp::mrb;                             // move/resize active
xwindow *qapp::focusclient;                 // currently active client
xwindow *qapp::tmaxclient;                  // maximized client in tiled mode
bool qapp::tdesks[10];                      // desks in tiled state
winfo *qapp::winf;
bool qapp::menu_open;                       // toolbar menu open
QMap <QString, int> qapp::cprops;           // client flags for Property command
QMap <QString, int> qapp::apclients;        // Toolbar clients, position number
QDateTime qapp::lmtime;                     // last defaults modification time 
bool qapp::sighup;                          // SIGHUP received
int qapp::servershapes;                     // server supports shapes

static int ShapeEventBase;                  // event base for shape extension
static bool rootptr = TRUE;                 // mouse pointer in root window

void sig_term(int);

qapp::qapp(int &argc, char **argv) : QApplication(argc, argv) 
{
	// get WM protocols required by ICCCM
	
	wm_protocols = XInternAtom(QX11Info::display(), "WM_PROTOCOLS", FALSE); 
	wm_delete = XInternAtom(QX11Info::display(), "WM_DELETE_WINDOW", FALSE);
	wm_change_state = XInternAtom(QX11Info::display(), "WM_CHANGE_STATE", FALSE);
	wm_state = XInternAtom(QX11Info::display(), "WM_STATE", FALSE);
	wm_take_focus = XInternAtom(QX11Info::display(), "WM_TAKE_FOCUS", FALSE);
	wm_resource_manager = XInternAtom(QX11Info::display(), "RESOURCE_MANAGER", FALSE);
	wm_colormaps = XInternAtom(QX11Info::display(), "WM_COLORMAP_WINDOWS", FALSE);

	// Extensions 
	kde_net_wm_system_tray_window_for = XInternAtom(QX11Info::display(), "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", FALSE);
	net_wm_name = XInternAtom(QX11Info::display(), "_NET_WM_NAME", FALSE);
	net_wm_icon_name = XInternAtom(QX11Info::display(), "_NET_WM_ICON_NAME", FALSE);
	net_supported = XInternAtom(QX11Info::display(), "_NET_SUPPORTED", FALSE);
        net_wm_window_opacity = XInternAtom(QX11Info::display(),"_NET_WM_WINDOW_OPACITY", FALSE );

	// save defaults modification time
	
	QString fname = get_cfile("defaults");

	if(! fname.isNull())
	{
		QFileInfo fi(fname);
		lmtime = fi.lastModified();
	}	

	// check if server supports nonrectangular windows
	
	int err;
	servershapes = XShapeQueryExtension(QX11Info::display(), &ShapeEventBase, &err);
}

void qapp::setinactive(xwindow *client)  // set last active client to inactive
{
	if(client != focusclient)
	{
	
		if(focusclient != NULL && clients.indexOf(focusclient) != -1)  // still there
			focusclient->setinactive();

		focusclient = client;
	}
}

void qapp::stopautofocus(void)
{
	if(focusclient != NULL && clients.indexOf(focusclient) != -1)
		focusclient->stopautofocus();
}

void qapp::run_client(Window w)  // start new client
{
	xwindow *client;
        int format;
	Atom type;
	unsigned long nitems=0;
	unsigned long extra=0;
	unsigned char *data=NULL;

	stopautofocus();

        qDebug() << "qapp::run_client";
	if((client = cwindows[w]) != NULL)
	{
		client->map();
                qDebug() << "start new client: just map ** ";
	}
	else  // new client
	{
		XClassHint ch;
		if(XGetClassHint(QX11Info::display(), w, &ch))
		{
			QString clname(ch.res_class);
			QString cclname(ch.res_name);
			cclname += ',';
			cclname += clname;
			
			XFree(ch.res_name);
			XFree(ch.res_class);
			XGetWindowProperty(QX11Info::display(), w, kde_net_wm_system_tray_window_for, 0, 1L, FALSE, XA_WINDOW, &type, &format, &nitems, &extra, &data);

			if(data != NULL)
				XFree(data);

		}
		clients.prepend((client = new xwindow(w)));

		if(client->is_tileable() && tdesks[adesk] && ! client->is_unmapped())
		{
			tile_order(tmaxclient);

                        client->show();
	    		XMapWindow(QX11Info::display(), client->client_id());
	    		client->set_clientstate(NormalState);                        
	    		XSync(QX11Info::display(), FALSE);
			client->raise();
                        qDebug()  << "New client created and raised **";
		}	
	}
}

void qapp::install_colormap(Colormap cmap)
{
	static Colormap lastmap = None;

	if(cmap == lastmap)
		return;

	lastmap = cmap;
		
	if(cmap == None)
		cmap = DefaultColormap(QX11Info::display(), DefaultScreen(QX11Info::display()));
		
	XInstallColormap(QX11Info::display(), cmap);
}

void qapp::wm_restart(void)
{
	xwindow *client;
	int i;

        //if(smode)
        //	keyboard::tscreen();

	for(i=0; i < defaults::vdesks; i++)
	{
		if(tdesks[i])
		{
                        HomeDesktop::instance()->getPager()->setActiveDesktop(i);
			toggle_tiled();
		}
	}

	winf->release_cancel();
        HomeDesktop::instance()->getPager()->setActiveDesktop(0);
	
	foreach(client, clients)
	{
		XRemoveFromSaveSet(QX11Info::display(), client->client_id());
		XReparentWindow(QX11Info::display(), client->client_id(), QX11Info::appRootWindow(), client->x(), client->y());
		XMapWindow(QX11Info::display(), client->client_id());
	}
	
	clients.clear();
	XSync(QX11Info::display(), FALSE);
	
	const char *argv[20];
	int rst=TRUE;

	for(i=0; i < defaults::argc && i < 18; i++)
	{
		argv[i] = defaults::argv[i];
			
		if(strcmp(argv[i], "-restart") == 0)
			rst = FALSE;
	}

	if(rst)
		argv[i++] = "-restart";
		
	argv[i] = NULL;

	execvp(argv[0], (char **)argv);
	perror(argv[0]);
	exit(1);
}

bool qapp::is_curdesk(xwindow *client)  // client on current desk
{
	if(client->x()+client->width()/2 > 0 && client->x()+client->width()/2 < QApplication::desktop()->width())
		return(TRUE);
		
	return(FALSE);
}

void qapp::tile_order(xwindow *actclient)
{
	if(smode)
		return;

	xwindow *client,*tmcl=NULL;

	int cct=0,i,cheight=0,lh=0;
	QWidget *dt = QApplication::desktop();

	if(actclient != NULL && (clients.indexOf(actclient) == -1 || ! actclient->is_tileable() || actclient->is_unmapped() || ! is_curdesk(actclient)))
		actclient = NULL;

	for(i=0; i < clients.size(); i++)
	{
		client = clients.at(i);
		if(client != actclient && ! client->is_unmapped() && is_curdesk(client) && client->is_tileable())
			cct++;
	}

	if(actclient == NULL && cct > 0)
		cct--;

	tmaxclient = NULL;
		
	if(cct)
		cheight = (dt->height()-defaults::tb_height)/cct;

	int xpos,xcw,ypos,yp;
	ypos = yp = defaults::toolbar_top?defaults::tb_height+1:0;
	xpos = (int)(dt->width()*defaults::tleftspace);
	xcw = dt->width()-xpos-1;
	
	foreach(client, clients)
	{
		if(! client->is_tileable() || client->is_unmapped() || ! is_curdesk(client)) 
			continue;

		if(actclient == NULL || cct == 0)
			actclient = client;
			
		if(client == actclient)
		{
			tmcl = tmaxclient = client;
			continue;
		}
		
		if(lh < 0)
			lh = 0;
			
		client->minimize_frame(cct > defaults::wminframe?TRUE:FALSE);
			
		lh = client->set_tile(xpos+1, ypos-lh, xcw, cheight+lh);
		ypos += cheight;
	}

	if(tmcl != NULL)
	{
		tmcl->minimize_frame(FALSE);
		tmcl->set_tile(0, yp, xpos, dt->height()-defaults::tb_height);
	}
	
	if(actclient != NULL)
		clients.prepend(clients.takeAt(clients.indexOf(actclient)));
}

void qapp::tile_maximize(xwindow *client)
{
	int i;
	
	if(tmaxclient != NULL && (i = clients.indexOf(tmaxclient)) != -1)
		clients.insert(clients.indexOf(client),  clients.takeAt(i));
		
	tile_order(client);
}

void qapp::toggle_tiled(void) // toggle overlapped/tiled desk 
{
	xwindow *client;
	
	if(smode)
		return;
	
	if(tdesks[adesk])
	{
		foreach(client, clients)
		{
			if(is_curdesk(client))
				client->unset_tile();
		}	
			
		tdesks[adesk] = FALSE;
		tmaxclient = NULL;

		if(focusclient != NULL && clients.indexOf(client) != -1)
			focusclient->focus_mouse();
			
		return;
	}
	tile_order(focusclient);
	tdesks[adesk] = TRUE;
}

void qapp::read_cprops(void)  // read app defaults
{
	QString fname,name,par;
	QByteArray cline;
	int flags;
	int apnum = 1;
	
	fname = get_cfile("appdefaults");
	
	if(fname.isNull())
		return;
	
	QFile istr(fname);
	
	if(! istr.open(QIODevice::ReadOnly))
	{
		perror("cannot open appdefaults");
		return;
	}	
	cprops.clear();
	apclients.clear();
	
	while(! istr.atEnd())
	{
		QTextStream si(istr.readLine(1024));
		si >> name;
		par = si.readLine();

		if(par.indexOf((QString("ToolBar")), Qt::CaseInsensitive) != -1)
		{
			apclients.insert(name, apnum++);
			continue;
		}
	
		flags = 0;	
		
		int i;
		int paf[] = { WindowListSkip,Sticky,SmallFrame,NoResize,NoTile,NoKey,StayOnBottom,NoScreen };
		const char *pas[] = { "WindowListSkip","Sticky","SmallFrame","NoResize","NoTile","NoKey","StayOnBottom","NoScreen" };

		for(i=0; i < 8; i++)
		{
			if(par.indexOf(pas[i]) != -1)
				flags |= paf[i];
		}
		
		if(flags)
			cprops.insert(name, flags);
	}
	istr.close();

	// check for clients to update
		
	for(int i=0; i < clients.size(); i++)
		clients.at(i)->set_pflags();
			        
}

QString qapp::get_cfile(const char *name)  // get abs config file name
{
	QString fname;
	
	if(! defaults::cfdir.isNull())  // user config dir
	{
		fname = defaults::cfdir;
		fname += '/';
		fname += name;

		QFileInfo fi(fname);

		if(fi.isReadable())
			return(fname);
	}
	
	fname = CONFDIR;   // system config dir
	fname += name;
	
	QFileInfo fi(fname);

	if(fi.isReadable())
		return(fname);
		
	perror((const char *)&fname);
	fname = QString();
	return(fname);
}

void qapp::send_configurenotify(xwindow *client) 
{
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.event = client->client_id();
	ce.window = ce.event;
	ce.x = client->x();
	ce.y = client->get_clienty();
	ce.width = client->width();
	ce.height = client->getcheight();
	ce.above = None;
	ce.border_width = 0;
	ce.override_redirect = 0;
	XSendEvent(QX11Info::display(), ce.window, False, StructureNotifyMask, (XEvent *) &ce);
}

// event filter returns FALSE to pass event to qt's event handler

bool qapp::x11EventFilter(XEvent *event)  
{
	xwindow *client;
	Window w;
	int i;
	XConfigureRequestEvent *cev;
	XClientMessageEvent *mev;
	XCrossingEvent *xev;
	XCirculateRequestEvent *rev;
	XPropertyEvent *pev;

#ifdef DEBUGMSG		
#include "eventnames.h"
	if(event->type < 36 && event->type != 6)
		logmsg << "Received: " << event_names[event->type] << " (WId:" << event->xany.window << ")\n";
#endif
	while(waitpid(-1, NULL, WNOHANG) > 0) ;

	if(sighup)
	{
		QString fname = get_cfile("defaults");

		if(! fname.isNull())
		{
			QFileInfo fi(fname);
			if(! (fi.lastModified() == lmtime || fi.lastModified().addSecs(180) < QDateTime::currentDateTime()))
				wm_restart();
				
			lmtime = fi.lastModified();	
		}

		read_cprops();
		sighup = FALSE;
	}

	switch(event->type)
	{
		case DestroyNotify:
			w = event->xdestroywindow.window;

			if((client = cwindows[w]) != NULL)
			{
				clients.removeAt(clients.indexOf(client));
				delete client;
				
				
				return TRUE;
			}	
				
			if(event->xdestroywindow.event != w)
				return TRUE;

                        if( w == home->winId() )
				sig_term(SIGTERM);

			return FALSE;

		case MapNotify:
			if(event->xmap.event != event->xmap.window)
				return TRUE;
		
                        if((client = pwindows[event->xmap.window]) != NULL){                            
                            client->show();                            
                        }



			return FALSE;

		case UnmapNotify:                        

			if((client = cwindows[event->xunmap.window]) != NULL)
			{
				if(event->xunmap.send_event)
				{
					// client requested transitions 
					// normal -> withdrawn
					// iconic -> withdrawn
					
					client->withdraw();
				}
				else
					client->unmap();
				
				return TRUE;	
			}
			if(event->xunmap.event != event->xunmap.window)
				return TRUE;

		
			return FALSE;

		case EnterNotify:                        
			xev = &event->xcrossing;
			
			if(event->xcrossing.window == QX11Info::appRootWindow())
			{
                          //  qDebug() << "Root window";
				stopautofocus();
				rootptr = TRUE;
			}
			else if(mrb == FALSE && menu_open == FALSE && (client = pwindows[xev->window]) != NULL && 
			((i = clients.indexOf(client))) != -1 && ((client = clients.at(i)) != focusclient || rootptr))
			{
				rootptr = FALSE;
				setinactive(client);  // old client to inactive, save new client
				
				if(xev->detail != NotifyInferior)
					client->startautofocus();

				client->setchildfocus(xev->time);
				client->setcmapfocus();
			}
			return FALSE;

		case ColormapNotify:
                        //qDebug() << "ColormapNotify";

			if((client = cwindows[event->xcolormap.window]) != NULL)
			{
				client->setcmap(event->xcolormap.colormap);
				return TRUE;
			}	
			return FALSE;

		case PropertyNotify:
                        //qDebug() << "PropertyNotify";

			pev = &event->xproperty;
#ifdef DEBUGMSG			
			logmsg << "Atom: " << XGetAtomName(QX11Info::display(), pev->atom) << '\n';
#endif
			
			if((client = cwindows[pev->window]) != NULL)
			{
				if(pev->atom == XA_WM_NORMAL_HINTS)
				{
					client->get_wmnormalhints();
				}	
				else if(pev->atom == XA_WM_HINTS)
				{
					client->get_wmhints();
				}
				else if(pev->atom == XA_WM_NAME || pev->atom == XA_WM_ICON_NAME || pev->atom == net_wm_name || pev->atom == net_wm_icon_name)
				{
					client->get_wmname();
				}
				else if(pev->atom == wm_colormaps)
				{
					client->get_colormaps();
					
					if(client == focusclient)
						client->setcmapfocus();
				}
				return TRUE;
			}
			return FALSE;

		case ConfigureNotify:
                        //qDebug() << "ConfigureNotify";
			if(event->xconfigure.event != event->xconfigure.window)
				return TRUE;
				
			return FALSE; 
			
		case ReparentNotify:
                        //qDebug() << "ReparentNotify:";

                        if((client = cwindows[event->xreparent.window]) != NULL &&
			event->xreparent.parent != client->winId())
			{
				clients.removeAt(clients.indexOf(client));                                
			}	
			return TRUE;
		

		case ButtonPress:
                        //qDebug() << "ButtonPress";

			w = event->xbutton.window;
			
                        if(w == QX11Info::appRootWindow())  // set focus to root window
                                XSetInputFocus(QX11Info::display(), w, RevertToPointerRoot, CurrentTime);

                        if( w == home->winId() /*|| w == home->getApbar()->winId()*/ ){
                                XRaiseWindow(QX11Info::display(), home->winId() );
                                //XSetInputFocus(QX11Info::display(), w, RevertToPointerRoot, CurrentTime);
                         }



			return FALSE;

		case ClientMessage:
                        //qDebug() << "ClientMessage";

                        mev = &event->xclient;
			
			if(mev->message_type == wm_change_state && mev->format == 32 && 
			mev->data.l[0] == IconicState && (client = cwindows[mev->window]) != NULL)
				client->iconify();

			return TRUE;	

		case CirculateRequest:
                        //qDebug() << "CirculateRequest";
			rev = &event->xcirculaterequest;

			if((client = cwindows[rev->window]) != NULL)
			{
				if(rev->place == PlaceOnTop)
					client->raise();
			}		
			else
				XLowerWindow(QX11Info::display(), rev->window);
				
			return TRUE;

		case ConfigureRequest:
                        //qDebug() << "COnfigureRequest";

			cev = &event->xconfigurerequest;
			XWindowChanges wc;
			
			if((client = cwindows[cev->window]) != NULL)
			{
#ifdef DEBUGMSG
	logmsg << "configure request to client (WId:" << client->winId() << ")\n";
#endif	
				if(cev->value_mask & (CWWidth|CWHeight|CWX|CWY))
				{

					if(! client->is_tiled() || client == tmaxclient)
					{	
						int cx,cy,cw,ch;

						if(cev->value_mask & CWWidth)
						cw = cev->width;
						else	
							cw = client->width();

						if(cev->value_mask & CWHeight)
							ch = cev->height;
						else
							ch = client->getcheight();

						if((cev->value_mask & CWX) && ! client->is_tiled())
							cx = cev->x;
						else
							cx = client->x();

						if((cev->value_mask & CWY) && ! client->is_tiled())
					    		cy = cev->y;
						else
							cy = client->y();
						
						client->resize_request(cx, cy, cw, ch);
					}
					cev->value_mask &= ~(CWWidth|CWHeight|CWX|CWY);
				}
				
				if(! cev->value_mask)
					return TRUE;

                                wc.width = client->width();
                                wc.height = client->height();
                                wc.x = client->x();
                                wc.y = client->y();
				wc.border_width = 0;
				wc.sibling = cev->above;
				wc.stack_mode = cev->detail;

				XConfigureWindow(QX11Info::display(), client->winId(), cev->value_mask, &wc);
				send_configurenotify(client);
			}
			else  // never mapped window
			{
                            //qDebug() << "Never Mapped Window";
                                if( cev->window == home->winId() )
					return TRUE;

#ifdef DEBUGMSG
	logmsg << "configure request to unreparented window (WId:" << cev->window << ")\n";

#endif	

				wc.x = cev->x;
				wc.y = cev->y;
				wc.width = cev->width;
				wc.height = cev->height;
				cev->value_mask &= (CWX|CWY|CWWidth|CWHeight);
				
				XConfigureWindow(QX11Info::display(), cev->window, cev->value_mask, &wc);	
			}
			return TRUE;
			
		case MapRequest:
                        //qDebug() << "MapRequest";
			run_client(event->xmaprequest.window);
			return TRUE;


		case KeyPress:                        
                        return TRUE;
		default:                        
			if(servershapes && event->type == (ShapeEventBase + ShapeNotify))
			{
				XShapeEvent *sev = (XShapeEvent *)event;
		
				if((client = cwindows[sev->window]) != NULL)
				{
					client->reshape();
					return TRUE;
				}	
			}
	}
	return FALSE;
}
