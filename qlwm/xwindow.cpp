/*
*  File      : xwindow.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  This is constructed for every normal window
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
#include "winfo.h"
#include "homedesktop.h"
#include "toolbarbutton.hh"
#include "xwindowtoolbar.h"
#include <QtDebug>
#include <QGraphicsRectItem>
#include <QGraphicsScene>


xwindow::xwindow(Window w, QWidget *parent) : QGraphicsView(parent)
{
        setGeometry(0,0,800,480);

        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFrameStyle( QFrame::NoFrame );
        setRenderHints(QPainter::Antialiasing);

        dt = QApplication::desktop();
        scene = new QGraphicsScene( 0, 0, dt->width(), dt->height() );
        setScene( scene );
        setBackgroundBrush( QBrush(QPixmap::grabWindow( HomeDesktop::instance()->winId() ) ) );

	dt = QApplication::desktop();
	maxstate = 0;
	clientid = w;
	actpal = TRUE;
	urgpal = FALSE;
	cmapwins = NULL;
	sstate = FALSE;
	tstate = FALSE;
	trsize = FALSE;
	ncmaps = 0;
	withdrawnstate = unmapped = TRUE;

	// get ClassHint
	get_classhint();
	clientname = res_class;

	// get flags for WM_COMMAND
	
	char **argv;
	int argc;
	if(XGetCommand(QX11Info::display(), w, &argv, &argc) && argc)
	{
		int ncm=0;
		while(1)
		{
			command += argv[ncm];

			if(argc > ++ncm)
				command += ' ';
			else
				break;
		}
		
		if(clientname.isEmpty() && ! command.isEmpty())
		{
			char *base;
			if((base = strrchr(argv[0], '/')) == NULL)
				base = argv[0];
			else
				base++;

			clientname = base;
		}	
		XFreeStringList(argv);
	}
	clientname = clientname.simplified();
	pflags = get_clientflags();
	
	if(pflags & qapp::SmallFrame)
	{
		uborderh = defaults::lowerborderheight;
		borderh = 2*uborderh;
	}
	else
	{
                uborderh = 85;//defaults::windowbuttonsize;
                borderh = 85/*defaults::windowbuttonsize*/+defaults::lowerborderheight;
	}

	// check for nonrectangular window
	shaped = (qapp::servershapes)?(query_shape()):(FALSE);

	if(pflags & qapp::NoResize)
		borderh -= defaults::lowerborderheight;

	// save window geometry
	Window root;
	uint bwidth,depth;
        XGetGeometry(QX11Info::display(), w, &root, &pos_x, &pos_y, &init_w, &init_h, &bwidth, &depth);
	base_w = init_w;
	base_h = init_h;
	init_h += borderh;
	
        qDebug()<< "window initial size: " << init_w << init_h;

	// reparent
	XSetWindowBorderWidth(QX11Info::display(), w, 0);
	XSetWindowBorderWidth(QX11Info::display(), winId(), 0);
        XReparentWindow(QX11Info::display(), w, winId(), 0, 85/*uborderh*/);
	XAddToSaveSet(QX11Info::display(), w);

	// get TransientForHint
	transfor = None;
	XGetTransientForHint(QX11Info::display(), w, &transfor);

        qDebug() << "Client transfor: " << transfor;

	// set Font
	setFont(defaults::borderfont);

	// get colormap and check for WM_COLORMAP_WINDOWS property
	get_colormaps();

	// get WM_CLIENT_MACHINE
	XTextProperty prop;
	if(XGetWMClientMachine(QX11Info::display(), w, &prop) && prop.nitems && prop.format == 8)
	{
		clientfqdn = (char *)prop.value;

		if(defaults::showclientmachines)
                {
			int pos = clientfqdn.indexOf('.');
			if(pos == -1)
				clientmachine = clientfqdn;
			else	
				clientmachine = clientfqdn.left(pos);
		}
	}	

	// get WMHints
	get_wmhints();

	// get WMNormalhints
	get_wmnormalhints();
	
        qDebug() << "base: " << base_w << base_h;
        qDebug() << "min: " << min_w << min_h;
        qDebug() << "max: " << max_w << max_h;

        int cw = init_w;
        int ch = init_h;
        getsize(&cw, &ch);
		
	// window position

	if(wmnflags & USPosition)
	{
		if(wmnflags & PWinGravity && 
			(wingrav == SouthWestGravity || wingrav == SouthEastGravity))
				pos_y -= uborderh;
	}

	// a transient window with program specified position looks like a dialog box,
	// otherwise use autoplacement
        /*

	else if(! defaults::start_restart && (transfor == None || ! (wmnflags & PPosition)))
	{
                qDebug() << "********************** 1";

		if(qapp::next_x+cw > dt->width())
		{
			pos_x = 0;
			if(cw < dt->width())
				qapp::next_x = 2*defaults::windowbuttonsize;
		}
		else
		{
			pos_x = qapp::next_x;
			qapp::next_x += 2*defaults::windowbuttonsize;
		}	

		int sy,ey;
		if(defaults::toolbar_top)
		{
			sy = defaults::tb_height;
			ey = dt->height();
		}
		else
		{
			sy = 0;
			ey = dt->height()-defaults::tb_height;
		}
		
		if(qapp::next_y+ch > ey)
		{
			pos_y = sy;
			if(ch < dt->height())
				qapp::next_y = sy+2*defaults::windowbuttonsize;
		}
		else
		{
			pos_y = qapp::next_y;
			qapp::next_y += 2*defaults::windowbuttonsize;
		}
	}	
        */
        qDebug() <<  "**************** TRANSIENT WINDOW:" << transfor;

        XResizeWindow(QX11Info::display(), clientid, 800, 395);

        if(pos_y < 0)
                pos_y = 0;
        setGeometry(0, 0, 800, 480);
    	// overwrite Qt-defaults because we need SubstructureNotifyMask	

	XSelectInput(QX11Info::display(), winId(),
                 //KeyPressMask | KeyReleaseMask |
  		 ButtonPressMask | ButtonReleaseMask |
  		 KeymapStateMask |
   		 ButtonMotionMask |
  		 PointerMotionMask |
		 EnterWindowMask | LeaveWindowMask |
  		 FocusChangeMask |
  		 ExposureMask |
		 StructureNotifyMask |
		 SubstructureRedirectMask |
		 SubstructureNotifyMask);


	XSetWindowAttributes attr;
	attr.event_mask = ColormapChangeMask|PropertyChangeMask;
	XChangeWindowAttributes(QX11Info::display(), w, CWEventMask, &attr);

	// get WM protocols
	getwmprotocols();


        // create window borders
        createWindowDecoration();

        // add client to lookup tables
	
	qapp::cwindows.insert(w, this);
	qapp::pwindows.insert(winId(), this);

	// get WM_NAME and set window title
	get_wmname();

	if(shaped)
		reshape();

	// init autofocus timer
	
	focustimer = new QTimer(this);
	Q_CHECK_PTR(focustimer);
	focustimer->setSingleShot(TRUE);
	connect(focustimer, SIGNAL(timeout()), SLOT(raise()));

	// inform client about NET extensions
        Atom data[4];
	int i=0;
	data[i++] = qapp::net_supported;
	data[i++] = qapp::net_wm_name;
	data[i++] = qapp::net_wm_icon_name;
        //data[i++] = qapp::net_wm_window_opacity;
	XChangeProperty(QX11Info::display(), w, qapp::net_supported, XA_ATOM, 32,
		PropModeReplace, (unsigned char *) data, i);

	
	qapp::send_configurenotify(this);
	
	if(! urgent && ! defaults::starturgent)
	{
		setinactive();
	}
	else 
	{
		seturgent();
	}	

	if(defaults::start_restart)  // restore original state
	{
		int clstate = get_clientstate();

		if(clstate == IconicState)
		{
			iconify();
		}	
		else if(clstate == WithdrawnState)
		{
			withdraw();
		}
		else map();
	}	
	else map();

        if ( transfor == None ){
            // Make window appear in pager
            qtablet::PagerDesktopItem * item = HomeDesktop::instance()->getPager()->addWindow( this->winId(), this->icaption() );
            if ( item ){
                connect( item, SIGNAL( showWindow( bool ) ), this, SLOT(state(bool)) );
            }
        }


#ifdef DEBUGMSG
	logmsg << "class xwindow constructed (WId:" << winId() << ")\n";
#endif	
}



void xwindow::createWindowDecoration(){

    toolbar = new XWindowToolbar( transfor != None );
    scene->addItem( toolbar );
    toolbar->setPos( 0, 0 );

    connect( toolbar, SIGNAL(minimizeWindow()), this, SLOT(iconify()) );
    connect( toolbar, SIGNAL(closeWindow()),    this, SLOT(wdestroy()) );
    connect( toolbar, SIGNAL(showMenu()),       this, SLOT(showHildonMenu()) );
}



void xwindow::getsize(int *pw, int *ph)   // adjust for possible width and height including border
{
	int w = *pw;
	int h = *ph;

	if(inc_w == 0)
		w = init_w;

	if(inc_h == 0)
		h = init_h;

	if(w > max_w)
		w = max_w;

	if(h > max_h)
		h = max_h;

	if(w < min_w)
		w = min_w;

	if(h < min_h)
		h = min_h;

	if(inc_w > 1)
	{
		int i = (w-base_w)/inc_w;
		w = base_w+i*inc_w;
		
		if(w < min_w)
			w += inc_w;
	}

	if(inc_h > 1)	
	{
		int j = (h-base_h)/inc_h;
		h = base_h+j*inc_h;
		
		if(h < min_h)
			h += inc_h;
	}	
	*pw = w;
	*ph = h;
}

void xwindow::resize_request(int cx, int cy, int cw, int ch)  // client requested resize
{
    qDebug() << "resize_request";
	if(cy-=uborderh < 0)
		cy = 0;
		
	ch += borderh;
	getsize(&cw, &ch);

        //setGeometry(cx, cy, cw, ch);
        setGeometry(0,0,800,480);
        XResizeWindow(QX11Info::display(), clientid, 800, 395);
        //XResizeWindow(QX11Info::display(), clientid, cw, ch-borderh);
	maxstate = 0;
}

void xwindow::resize_client(int px, int py, int pw, int ph)  // dimensions include borders
{

	int nw = width();
	int nh = height();
        qDebug() << "resize_client";
        setGeometry(0,0,800,480);
        XResizeWindow(QX11Info::display(), clientid, 800, 395);
        Q_UNUSED( px );
        Q_UNUSED( py );
        Q_UNUSED( pw );
        Q_UNUSED( ph );
        // Resize this xwindow object
        //if(px != x() || py != y() || pw != nw || ph != nh)  // parent
        //	setGeometry(px, py, pw, ph);

        // resize the xclient under to border
        if(pw != nw || ph != nh)
	{
                //XResizeWindow(QX11Info::display(), clientid, pw, ph-borderh);
	}


}

/*
void xwindow::t_maximize(void)
{
	int cw,ch;
	
	if(qapp::smode)
	{
		focus_mouse();
		return;
	}

	if(maxstate != 1)  // maximize
	{
		if(maxstate == 0)
		{
			icx = x();
			icy = y();
			icw = width();
			ich = height();
		}

		if(defaults::tmx1 == -1 || tstate || ((pflags & qapp::NoTile) && qapp::is_tileddesk()))   // not user configured
		{
			cw = dt->width();
			ch = dt->height()-defaults::tb_height-1;
			getsize(&cw, &ch);
		
			resize_client(0, defaults::toolbar_top?defaults::tb_height+1:0, cw, ch);
		}
		else
		{
			cw = defaults::tmx2-defaults::tmx1;
			ch = defaults::tmy2-defaults::tmy1;
			getsize(&cw, &ch);

			resize_client(defaults::tmx1, defaults::tmy1, cw, ch);
		}
		maxstate = 1;
	}
	else
	{
		resize_client(icx, icy, icw, ich);
		maxstate = 0;
	}
	raise();
}

*/
void xwindow::s_maximize(void)
{    
	int cw,ch;
	
	if(qapp::smode)
	{
            qDebug() << "xwindow::s_maximize: 1";
		focus_mouse();
		return;
	}

        if(maxstate != 2)  // maximize
	{
            qDebug() << "xwindow::s_maximize";
		if(maxstate == 0)                    
		{
                    qDebug() << "xwindow::s_maximize: 2";
			icx = x();
			icy = y();
			icw = width();
			ich = height();
		}

		if(defaults::smx1 == -1 || ((pflags & qapp::NoTile) && qapp::is_tileddesk()))
		{
                    qDebug() << "xwindow::s_maximize: 3";
			cw = dt->width();
			ch = dt->height();
			getsize(&cw, &ch);
		
			resize_client(0, 0, cw, ch);
		}
		else
		{
                    qDebug() << "xwindow::s_maximize: 4";
			cw = defaults::smx2-defaults::smx1;
			ch = defaults::smy2-defaults::smy1;
			getsize(&cw, &ch);

			resize_client(defaults::smx1, defaults::smy1, cw, ch);
		}
		maxstate = 2;
	}
	else
	{
            qDebug() << "xwindow::s_maximize: 4";
		resize_client(icx, icy, icw, ich);
		maxstate = 0;
	}
	raise();
}
void xwindow::show_info(void)
{
	qapp::winf->show_info(this, wmname, clientfqdn, command, res_name, res_class, inc_w, inc_h, base_w, base_h);
}

void xwindow::unscreen(void)
{
    qDebug() << "xwindow::unscreen()";

	if(sstate)  // screen mode
	{
		resize_client(scx, scy, scw, sch);
		sstate = FALSE;
	}
}

bool xwindow::is_tileable(void)
{
	if((pflags & (qapp::Sticky|qapp::NoTile)) || transfor != None)
		return FALSE;

	return TRUE;	
}

/*
void xwindow::toggle_tiled(void)
{
	qapp::toggle_tiled();
}
*/

void xwindow::minimize_frame(bool mf)
{
    qDebug() << "xwindow::minimize_frame";
	if(pflags & qapp::SmallFrame)
		return;

	int ch = height()-borderh;
		
	if(mf)
	{
		if(uborderh == defaults::lowerborderheight)
			return;


                uborderh = 85;//defaults::lowerborderheight;
		borderh = 2*uborderh;
	}
	else
	{
		if(uborderh == defaults::windowbuttonsize)
			return;


                uborderh = 85;//defaults::windowbuttonsize;
                borderh = 85/* defaults::windowbuttonsize*/ + defaults::lowerborderheight;
		set_title();
	}
	
	if(shaped || (pflags & qapp::NoResize))
		borderh -= defaults::lowerborderheight;

	get_wmnormalhints();
	resize(width(), ch+borderh);
        XMoveWindow(QX11Info::display(), clientid, 0, uborderh);
}

int xwindow::set_tile(int cx, int cy, int cw, int ch)
{
	if(tstate == FALSE)
	{
		tcx = x();
		tcy = y();
		tcw = width();
		tch = height();

		tstate = TRUE;
	}
	maxstate = 0;
	
	int cw2 = cw;
	int ch2 = ch;
	
	if(! trsize)
	{
		getsize(&cw2, &ch2);

		if(cw2 > cw)
			cw2 = cw;

		if(ch2 > ch)
			ch2 = ch;
		
		resize_client(cx, cy, cw2, ch2);
	}	
	raise();

	return(ch - ch2);
}

void xwindow::unset_tile(void)
{
	if(! tstate)
		return;

	resize_client(tcx, tcy, tcw, tch);
	maxstate = 0;
	minimize_frame(FALSE);
	
	tstate = FALSE;
}

void xwindow::setinactive(void)
{

	if(actpal || urgpal)
	{
		setPalette(*qapp::ipal);  // set inactive colors
		stopautofocus();
		actpal = FALSE;
	}

	if(urgpal)
	{                
		urgpal = FALSE;
	}	
}

void xwindow::setactive(void)
{
	if(urgpal || ! actpal)
	{
		setPalette(QApplication::palette());  // set standard colors
		actpal = TRUE;
	}

	if(urgpal)
	{                
		urgpal = FALSE;
	}	
}

void xwindow::seturgent(void)
{
	if(! urgpal)
	{
		setPalette(*qapp::upal);                
		urgpal = TRUE;
	}
}

void xwindow::raise()
{
        if( qapp::winf->get_client() != NULL || (! qapp::smode && (pflags & qapp::StayOnBottom)))
		return;
		
#ifdef DEBUGMSG
	logmsg << "raise (WId:" << winId() << ")\n";
#endif

	trsize = FALSE;
	XRaiseWindow(QX11Info::display(), winId());
	return;
}

void xwindow::map(void)
{
	withdrawnstate = whidden = FALSE;	

	if(map_iconic)  // InitialState WMHint
	{
                qDebug() << "xwindow::map";                

		set_clientstate(IconicState);

		if(urgent || urgpal)
		{
			urgpal = FALSE;
			seturgent();
		}	
	}	
	else
	{
                if(! isVisible()){                        
                    state( true );
                }
		map_normal();                

	}	

        if ( transfor == None ){
            // We can handle the old clients here also i.e. after restart of qlwm
            qtablet::PagerDesktopItem * item = HomeDesktop::instance()->getPager()->addWindow( this->winId(), this->icaption() );  // add to procbar
            if ( item != 0 ){
                connect( item, SIGNAL( showWindow( bool ) ), this, SLOT(state(bool)) );
            }
        }
}

void xwindow::map_normal(void)
{
	if(! isVisible())
	{
#ifdef DEBUGMSG
		logmsg << "map client\n";
#endif	               
                qDebug()<< this->winId() << QX11Info::appRootWindow();

		unmapped = FALSE;
		
		if(is_tileable() && qapp::is_tileddesk()) 
		{
			if(clients.indexOf(this) == -1)
				return;

			qapp::tile_order(trsize?this:qapp::tmaxclient);
		}	
		else  // tiled window mapped on untiled desk
		{
			unset_tile();
		}

		
		// set position to visible area
                /*
		if(x() >= dt->width())
		{
			move((x())%(dt->width()), y());
		}
		else if(x() < -(dt->width()/2))
		{
			move(dt->width()+(x()+1)%(dt->width()), y());
		}
                */

                show();
		XMapWindow(QX11Info::display(), clientid);
		set_clientstate(NormalState);

		XSync(QX11Info::display(), FALSE);

	}

	if(pflags & qapp::StayOnBottom)
		XLowerWindow(QX11Info::display(), winId());
        else{                
		raise();
            }
}

void xwindow::unmap(void)
{
	stopautofocus();

	if(! isVisible())
		return;

	unmapped = TRUE;	
	
	hide();
	XUnmapWindow(QX11Info::display(), clientid);

	if(tstate && qapp::is_curdesk(this))
	{
		if(qapp::tmaxclient == this)
			qapp::tmaxclient = NULL;
			
		qapp::tile_order(qapp::tmaxclient);
	}
}

void xwindow::iconify(void)  // transition to iconic
{
        qDebug() << "xwindow::iconify";
	if(qapp::tmaxclient == this && qapp::is_tileddesk())
		trsize = TRUE;

        // Update the iconified screenshot
        if ( transfor == None ){
            HomeDesktop::instance()->getPager()->addWindow( this->winId(), this->icaption() );
        }

	unmap();
	withdrawnstate = FALSE;
	set_clientstate(IconicState);


}

void xwindow::whide(void)  
{
	unmap();
	withdrawnstate = FALSE;
	set_clientstate(IconicState);        
	whidden = TRUE;
}

void xwindow::withdraw(void)
{
	unmap();
	withdrawnstate = TRUE;
	set_clientstate(WithdrawnState);

	
#ifdef DEBUGMSG
	logmsg << "changed to withdrawn (WId:" << winId() << ")\n";
#endif
}

void xwindow::focus_mouse(bool wlist)  // map and set mouse (generates enter event -> focus)
{
	int pw,ph;
	
	if(qapp::smode)  // screen mode
	{
		if(! isVisible())
			return;
			
		if(sstate == FALSE)
		{
			scx = x();
			scy = y();
			scw = width();
			sch = height();

			pw = dt->width();
			ph = dt->height()+uborderh+defaults::lowerborderheight;
			setGeometry(0, -uborderh, pw, ph);
			
			if(inc_w > 1)
			{
				int i = (pw-base_w)/inc_w;
				pw = base_w+i*inc_w;
			}

			if(inc_h > 1)	
			{
				int j = (ph-base_h)/inc_h;
				ph = base_h+j*inc_h;
			}	

                        //XResizeWindow(QX11Info::display(), clientid, pw, ph-borderh);
			sstate = TRUE;
		}
		raise();
		return;
	}
	map();	
	
	int mid = x()+(width()/2);
	
	if(mid > dt->width())
	{
                //HomeDesktop::instance()->getPager()->change_desk(qapp::adesk+(x()/(dt->width()-1)));
	}	
	else if(mid < 0)
	{
                //HomeDesktop::instance()->getPager()->change_desk(qapp::adesk+(x()/(dt->width()+1))-1);
	}
	else if(defaults::maxontab && ! wlist && tstate)
	{
		// access in tiled state will swap windows after timeout
		
		qapp::stopautofocus();
                //tfocustimer->start(defaults::maxontab);
	}	
	XWarpPointer(QX11Info::display(), None, winId(), 0, 0, 0, 0, width()/2, uborderh/2);
}

void xwindow::setchildfocus(long timestamp) // set to active and focus to child
{
#ifdef DEBUGMSG
	logmsg << "change active client to (WId:" << winId() << ")\n";
#endif

	setactive();

	if(inputfield)  // InputHint
		XSetInputFocus(QX11Info::display(), clientid, RevertToPointerRoot, CurrentTime);
		
	if(prot_takefocus)  // WM_TAKE_FOCUS protocol
		send_wmprotocol(qapp::wm_take_focus, timestamp);
}

void xwindow::stopautofocus(void)
{
	if(defaults::autofocustime)
		focustimer->stop();

        //if(tstate)
        //	tfocustimer->stop();
}

void xwindow::startautofocus(void)  // start autofocus timer
{
        if(! defaults::autofocustime )
		return;
		
	// do not raise if another window appears within this window

	Window w1,w2,*wins;
	uint nwins,i;
	XWindowAttributes attr;
	
	if(XQueryTree(QX11Info::display(), QX11Info::appRootWindow(), &w1, &w2, &wins, &nwins) && nwins)
	{
		for(i=0; i < nwins; i++)  // find current window
			if(wins[i] == winId())
				break;
				
		int cx2 = x()+width(); int cy2 = y()+height();
		
		while(++i < nwins)
		{
			XGetWindowAttributes(QX11Info::display(), wins[i], &attr);

			if(attr.map_state !=  IsViewable)
					continue;

			int nx2 = attr.x+attr.width; int ny2 = attr.y+attr.height;
			
			if(attr.x >= x() && nx2 <= cx2 && attr.y >= y() && ny2 <= cy2)
			{
				XFree(wins);
				return;
			}	
		}	
		XFree(wins);
	}	
	focustimer->start(defaults::autofocustime);
}

void xwindow::getwmprotocols(void)
{
	Atom *protocols;
	int nprot,i;
	
	prot_delete = FALSE;
	prot_takefocus = FALSE;

	if(XGetWMProtocols(QX11Info::display(), clientid, &protocols, &nprot))
	{
		for(i=0; i < nprot; i++)
		{
			if(protocols[i] == qapp::wm_delete)
			{
				prot_delete = TRUE;
			}	
			else if(protocols[i] == qapp::wm_take_focus)
			{
				prot_takefocus = TRUE;
			}
		}
		XFree(protocols);
	}
}

void xwindow::send_wmprotocol(long data0, long data1)  // send protocol message to child window
{
	XEvent event;
	
	memset(&event, 0, sizeof(event));
	event.xclient.type = ClientMessage;
	event.xclient.window = clientid;
	event.xclient.message_type = qapp::wm_protocols;
	event.xclient.format = 32;
	event.xclient.data.l[0] = data0;
	event.xclient.data.l[1] = data1;
	
	XSendEvent(QX11Info::display(), clientid, False, 0L, &event);
}

int xwindow::get_clientstate(void)
{
	Atom ret_type;
	int format;
	unsigned long n,extra;
	uchar *data;
	int ret_state;

	if(XGetWindowProperty(QX11Info::display(), clientid, qapp::wm_state, 0L, 1L, FALSE, qapp::wm_state, &ret_type, &format, &n, &extra, &data) != Success || n == 0)
	{
		logmsg << "WM: cannot get wm_state for " << clientid << '\n';
		return(None);
	}	
		
	ret_state = (int)*data;
	XFree((char *)data);
	
	return(ret_state);
}

void xwindow::set_clientstate(int state)
{
	ulong data[2];
	data[0] = (ulong)state;
	data[1] = (ulong)None;

	XChangeProperty(QX11Info::display(), clientid, qapp::wm_state, qapp::wm_state, 32, PropModeReplace, (uchar *)data, 2);
}

void xwindow::wdestroy(void)  // destroy client
{
	if(prot_delete)  // soft way
	{
		send_wmprotocol(qapp::wm_delete, CurrentTime);
#ifdef DEBUGMSG
		logmsg << "soft kill\n";
#endif
	}
	else 
	{ 
		XKillClient(QX11Info::display(), clientid);
		XSync(QX11Info::display(), FALSE);
#ifdef DEBUGMSG
		logmsg << "hard kill\n";
#endif		
	}	
}

void xwindow::get_colormaps(void)
{
	Window *w;
	int nm=0;

	XWindowAttributes attr;
	XGetWindowAttributes(QX11Info::display(), clientid, &attr);
	cmap = attr.colormap;

	if(! XGetWMColormapWindows(QX11Info::display(), clientid, &w, &nm))
		return;

	if(nm > 1000)
	{
		logmsg << "More than 1000 colormaps for " << clientname << " - property rejected\n";
		return;
	}	
	
	if(ncmaps) 
		delete [] cmapwins;

	if(nm == 0)
	{
		cmapwins = NULL;
		ncmaps = 0;
		
		return;
	}

#ifdef DEBUGMSG
	logmsg << "client has WM_COLORMAP_WINDOWS property\n";
#endif	
		
	cmapwins = new Window[nm];
	Q_CHECK_PTR(cmapwins);
	ncmaps = nm;
	
	for(int i=0; i < nm; i++) 
		cmapwins[i] = w[i];
		
	XFree(w);
}

void xwindow::setcmapfocus(void)
{
	XWindowAttributes attr;
	bool installed = FALSE;

	for(int i = ncmaps-1; i >= 0; i--)
	{
		Window w = cmapwins[i];
		
		if(w == clientid)
			installed = TRUE;
			
		XGetWindowAttributes(QX11Info::display(), w, &attr);
		qapp::install_colormap(attr.colormap);
	}	
    	if(! installed)
   		qapp::install_colormap(cmap);
}

bool xwindow::query_shape(void)
{
	int ns,order;

	XFree(XShapeGetRectangles(QX11Info::display(), clientid, ShapeBounding, &ns, &order));
	XShapeSelectInput(QX11Info::display(), clientid, ShapeNotifyMask);
	
	if(ns > 1)
		return TRUE;

	return FALSE;
}

void xwindow::reshape(void)
{
#ifdef DEBUGMSG
	logmsg << "reshaping client\n";
#endif

	shaped = TRUE;
	XShapeCombineShape(QX11Info::display(), winId(), ShapeBounding, 0, uborderh, clientid, ShapeBounding, ShapeSet);

	XRectangle tr;
	
	tr.x = tr.y = 0;
	tr.width = width();
	tr.height = uborderh;
	XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &tr, 1, ShapeUnion, Unsorted);

	if(! (pflags & qapp::NoResize))
	{
		tr.y = height()-defaults::lowerborderheight;
		tr.height = defaults::lowerborderheight;
		XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &tr, 1, ShapeUnion, Unsorted);
	}
}

void xwindow::get_classhint(void)
{
	XClassHint ch;
	
	if(XGetClassHint(QX11Info::display(), clientid, &ch))
	{
		res_name = ch.res_name;
		res_class = ch.res_class;
		
		XFree(ch.res_name);
		XFree(ch.res_class);
		
#ifdef DEBUGMSG
	logmsg << "res_name set to " << res_name << '\n';
#endif

	}	
}

int xwindow::get_clientflags(void)
{
	QString cclname(res_name);   // check for name,class format
	cclname += ',';
	cclname += res_class;
	int clf = qapp::cprops[cclname];

	if(clf)
		return(clf);

	if((clf = qapp::cprops[clientname]))
		return(clf|qapp::ClassOnly);

	return(0);	
}	

void xwindow::set_pflags(int tflags)
{
	int flags;
	
	if(tflags == -1)
		flags = get_clientflags();
	else
		flags = tflags;

	if(flags == qapp::StayOnBottom)
		XLowerWindow(QX11Info::display(), winId());
	
	if(flags == pflags)
		return;

	int oldflags = pflags;
	pflags = flags;

	if(pflags & (qapp::NoTile|qapp::Sticky))
		unset_tile();

	int mask = qapp::SmallFrame|qapp::NoResize;
	
       	if((flags & mask) != (oldflags & mask))  // rebuild window frame
	{
	
#ifdef DEBUGMSG
	logmsg << "rebuilding window frame (WId:" << winId() << ")\n";
#endif	
                qDebug() << "rebuilding window frame";


		
		int ch = height()-borderh;
		
		if(pflags & qapp::SmallFrame)
		{
                        uborderh = 85;//defaults::lowerborderheight;
			borderh = 2*uborderh;
		}
		else
		{
                        uborderh = 85;//defaults::windowbuttonsize;
                        borderh = 85;/*defaults::windowbuttonsize+defaults::lowerborderheight;*/
		}
	
		if(pflags & qapp::NoResize)
			borderh -= defaults::lowerborderheight;

		get_wmnormalhints();
		resize(width(), ch+borderh);
                XMoveWindow(QX11Info::display(), clientid, 0, uborderh);
		
                //createWindowDecoration();
		wmname = "";
		get_wmname();

		if(shaped)
			reshape();

                /*
		if(ubdr != NULL)
			ubdr->show();
		else
			midmove->show();

		if(lbdr != NULL)
			lbdr->show();
                */
	}

	if(! (oldflags & qapp::Sticky) && (flags & qapp::Sticky))  // move to current desk
	{
		int dx = x()/dt->width();

		if(x() < 0)
			dx--;
			
		move(x()-(dx*dt->width()), y());
	}
}

void xwindow::get_wmnormalhints(void)
{
	XSizeHints *xsizehints;
	if((xsizehints = XAllocSizeHints()) == NULL)
		sleep(1);

#ifdef DEBUGMSG
	logmsg << "reading WMNormalHints (WId:" << winId() << ")\n";
#endif	
		
	long hints;
	if(XGetWMNormalHints(QX11Info::display(), clientid, xsizehints, &hints) == 0)
		xsizehints->flags = 0;

	wmnflags = xsizehints->flags;
	wingrav = xsizehints->win_gravity;
		
	bool pminsize = FALSE;
	
	// set max,min and base size, results include window borders

	if(wmnflags & PMinSize) 
	{
		min_w = xsizehints->min_width;
		min_h = xsizehints->min_height+borderh;
		pminsize = TRUE;   // to get base size if unspecified
	} 
	else 
	{
		min_w = 4;
		min_h = borderh;
	}
	if(wmnflags & PMaxSize)
	{
		max_w = xsizehints->max_width;
		max_h = xsizehints->max_height+borderh;
		
		if(max_w > dt->width())
			max_w = dt->width();

		if(max_h > dt->height())
			max_h = dt->height();
	}
	else
	{
		max_w = dt->width();
		max_h = dt->height();
	}
	
	if(wmnflags & PBaseSize)
	{
		base_w = xsizehints->base_width;
		base_h = xsizehints->base_height+borderh;

		if(! pminsize)  // get from base if unspecified
		{
			min_w = base_w;
			min_h = base_h;
		}
	}
	else if(pminsize)
	{
		base_w = xsizehints->min_width;
		base_h = xsizehints->min_height;
	}
	
	if(wmnflags & PResizeInc)
	{
		inc_w = xsizehints->width_inc;
		inc_h = xsizehints->height_inc;
	}
	else
		inc_w = inc_h = 1;
		
	XFree(xsizehints);
}		

void xwindow::get_wmhints(void)  // get WMHints
{
	XWMHints *xwmhints;
	map_iconic = FALSE;
	inputfield = TRUE;
	urgent = FALSE;
	
	if((xwmhints = XGetWMHints(QX11Info::display(), clientid)) != NULL)
	{
#ifdef DEBUGMSG
	logmsg << "reading WMHints (WId:" << winId() << ")\n";
#endif	                

		if(xwmhints->flags & StateHint && xwmhints->initial_state == IconicState)
			map_iconic = TRUE;

		if(! (xwmhints->flags & InputHint))  // focus
			inputfield = FALSE;
			
                if(xwmhints->flags & IconPixmapHint){                    
			get_servericon(xwmhints->icon_pixmap, (xwmhints->flags & IconMaskHint)?(xwmhints->icon_mask):(None));

                    }
		if(xwmhints->flags & (1L << 8))
		{
			urgent = TRUE;
			seturgent();  // set urgent colors on window border and icon
		}
		else if(urgpal)
		{
			if(actpal)
				setactive();
			else
				setinactive();
		}
		XFree(xwmhints);
	}
}

void xwindow::set_title(void)
{
	if((pflags & qapp::SmallFrame) || (uborderh == defaults::lowerborderheight))
		return;

	QString *wmn = &wmname;
	QString tnm;

	if(tnumber)
	{
		QTextStream tns(&tnm);
		
		tns << wmname << '<' << tnumber << '>';
		wmn = &tnm;
	}
        if ( transfor == None ){
            toolbar->setTitle(*wmn);
        }

	setPalette(actpal?QApplication::palette():*qapp::ipal);
}

void xwindow::get_wmname(void)  // get WM_NAME and ICON_NAME and set caption and border pixmap
{
	char *name;
	xwindow *client;
	QString oldwname = wmname;
	QString oldiname = icname;
	Atom type;
	int format;
	unsigned long nitems=0;
	unsigned long extra=0;
	unsigned char *data=NULL;

	XGetWindowProperty(QX11Info::display(), clientid, qapp::net_wm_name, 0, 500, FALSE, AnyPropertyType, &type, &format, &nitems, &extra, &data);
	
	if(data != NULL)
	{
		wmname = QString::fromUtf8((char *)data);
		XFree(data);
	}
	else if(XFetchName(QX11Info::display(), clientid, &name) && name != NULL)
	{
		wmname = name;
		XFree(name);
	}
	else  // use class hints
		wmname = res_name;

	data = NULL;
	XGetWindowProperty(QX11Info::display(), clientid, qapp::net_wm_icon_name, 0, 500, FALSE, AnyPropertyType, &type, &format, &nitems, &extra, &data);
	
	if(data != NULL)
	{
		icname = QString::fromUtf8((char *)data);
		XFree(data);
	}
	else if(XGetIconName(QX11Info::display(), clientid, &name) && name != NULL)
	{
		icname = name;
		XFree(name);
	}
	else 
		icname = wmname;
		
#ifdef DEBUGMSG
	logmsg << "WM_NAME set to " << wmname << '\n';
#endif

	if(wmname != oldwname)
	{
		// look for other windows with the same title
	
		tnumber = 0;

		QList<xwindow*> clst = clients;
		
		for(int i=0; i < clst.size(); i++)
		{
			client = clst.at(i);
			if(client != this && ! (client->get_pflags() & qapp::SmallFrame) && wmname == client->ccaption() && client->get_tnumber() >= tnumber)
				tnumber = client->get_tnumber()+1;
		}
		set_title();
	}	
		
        //if(icname != oldiname)  // change icon text on procbar
                //HomeDesktop::instance()->getProcbar()->change_text(&icname, this);
}

void xwindow::get_servericon(Pixmap icon, Pixmap mask)  // get pixmap from server and scale it
{
	int ix,iy;
	uint iw,ih,bwidth,depth;
	Window w;
	GC gc;

	if(! XGetGeometry(QX11Info::display(), icon, &w, &ix, &iy, &iw, &ih, &bwidth, &depth))
		return;

	QPixmap pix(iw, ih);
	pix.detach();
	gc = XCreateGC(QX11Info::display(), icon, 0, 0);
	XCopyArea(QX11Info::display(), icon, pix.handle(), gc, 0, 0, iw, ih, 0 ,0);
	XFreeGC(QX11Info::display(), gc);

	if(mask != None)
	{
		gc = XCreateGC(QX11Info::display(), mask, 0, 0);
		QBitmap bmap(iw, ih);
		bmap.detach();
		XCopyArea(QX11Info::display(), mask, bmap.handle(), gc, 0, 0, iw, ih, 0, 0);
		pix.setMask(bmap);
		XFreeGC(QX11Info::display(), gc);
	}
	wicon = pix.scaledToHeight(defaults::tc_height-4, Qt::SmoothTransformation);
}

void xwindow::state(bool on)
{
	if(on)
		map_normal();
	else
		iconify();
}

xwindow::~xwindow(void)
{
    if ( transfor == None ){
        HomeDesktop::instance()->getPager()->removeWindow(this->winId());
    }

    delete [] cmapwins;


    if(qapp::winf->get_client() == this)
            qapp::winf->release_cancel();

    qapp::cwindows.remove(clientid);
    qapp::pwindows.remove(winId());

#ifdef DEBUGMSG
	logmsg << "class xwindow destructed (WId:" << winId() << ")\n";
#endif	
}

void xwindow::showHildonMenu(){

    qDebug() << "showHildonMenu()";
    XUngrabPointer(QX11Info::display(), CurrentTime);

  XEvent ev;

  memset(&ev, 0, sizeof(ev));

  ev.xclient.type = ClientMessage;
  ev.xclient.window = clientid;
  ev.xclient.message_type = qapp::mb_grab_transfer;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = CurrentTime;
  ev.xclient.data.l[1] = winId();
  ev.xclient.data.l[2] = 0;
  ev.xclient.data.l[3] = 0;
  ev.xclient.data.l[4] = 0;

  XSendEvent(QX11Info::display(), clientid, False, NoEventMask, &ev);

  XSync( QX11Info::display(), False);

}



