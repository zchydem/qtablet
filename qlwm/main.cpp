/* main.cpp */

#include "defs.h"
#include "conf.h"
#include "keyboard.h"
#include "winfo.h"
#include "qapp.h"
#include "defaults.h"

bool wm_init=TRUE;

void getrunprocs(void)  // get already running clients
{
	Window w,w1,w2,*wins;
	uint nwins,cwin;
	XWindowAttributes attr;

	if(XQueryTree(QX11Info::display(), QX11Info::appRootWindow(), &w1, &w2, &wins, &nwins) == 0 || ! nwins)
		return;
	
	bool surgent = defaults::starturgent;
	defaults::starturgent = FALSE;

	for(cwin=0; cwin < nwins; cwin++)
	{
		w = wins[cwin];
		
                if(w == qapp::home->winId()  )
			continue;

		XGetWindowAttributes(QX11Info::display(), w, &attr);

		if(attr.map_state == IsViewable && ! attr.override_redirect)
			qapp::run_client(w);
	}
	XSync(QX11Info::display(), FALSE);
	defaults::starturgent = surgent;
}

void sig_hup(int)
{
	qapp::sighup = TRUE;
}

void sig_term(int)  // terminate
{
        HomeDesktop::instance()->getProcbar()->remove_all();
	
	for(int i=0; i < clients.size(); i++)
		XReparentWindow(QX11Info::display(), clients.at(i)->winId(), QX11Info::appRootWindow(), 0, 0);
		
	XSync(QX11Info::display(), FALSE);

	exit(0);
}

void startprg(void)  // start programs from init stack
{
	pid_t pid;
	QByteArray *prg;
	while(! defaults::initexec.isEmpty())
	{
		prg = defaults::initexec.pop();

		if((pid = fork()) == 0)
		{
			execl("/bin/sh", "sh", "-c", prg->data(), NULL);
			perror("Exec");
			exit(1);
		}
		
		if(pid == -1)
			perror("fork");

		delete prg;	
	}
}

void make_bpixmaps(void)   // create scaled window button pixmaps
{
	int wh = defaults::windowbuttonsize-4;
	QImage limg(qapp::get_cfile("button_left.xpm"));
	QImage rimg(qapp::get_cfile("button_right.xpm"));

	qapp::leftwinpix = new QPixmap();
	qapp::rightwinpix = new QPixmap();
	
	if(! limg.isNull())
		*qapp::leftwinpix = QPixmap::fromImage(limg.scaled(wh, wh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	
	if(! rimg.isNull())
		*qapp::rightwinpix = QPixmap::fromImage(rimg.scaled(wh, wh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	
}

int xerrors(Display *d, XErrorEvent *event)
{
	char	msg[100];
	char	req[100];
	char	nmb[100];

	if(wm_init == TRUE && event->error_code == BadAccess)
	{
		logmsg << "Another WM is already running\n";
		exit(1);
	}

#ifndef DEBUGMSG
	if(event->error_code == BadWindow || event->error_code == BadMatch)
		return 0;
#endif

	XGetErrorText(d, event->error_code, msg, sizeof(msg));
	sprintf(nmb, "%d", event->request_code);
	XGetErrorDatabaseText(d, "XRequest", nmb, nmb, req, sizeof(req));

	logmsg << "WM: " << req << " on resource " << event->resourceid 
	<< " failed: " << msg << '\n';
		
	return 0;
}

int main(int argc, char **argv)
{
	for(int i=1; i < argc; i++)
	{
		if(strcmp(argv[i], "--version") == 0)
		{
			logmsg << DVERSION;
			return(0);
		}
		
		if(strcmp(argv[i], "-restart") == 0)
			defaults::start_restart = TRUE;

		if(strcmp(argv[i], "-c") == 0 && argc > i+1)
			defaults::cfdir = argv[++i];
	}

	if(defaults::cfdir.isNull())  // get config dir
	{
		QString fname(getenv("HOME"));

		if(! fname.isNull())
			defaults::cfdir = fname + "/.qlwm";
	}
	defaults::argc = argc;
	defaults::argv = argv;
	
	qapp a(argc, argv);
        a.setOverrideCursor( Qt::BlankCursor );

	defaults::read_config();

        qapp::home = HomeDesktop::instance();
	qapp::read_cprops();
	make_bpixmaps();
	qapp::winf = new winfo(0);
	
	XSetErrorHandler(xerrors);

	XIconSize *isize;
	if((isize = XAllocIconSize()) != NULL)
	{
		isize->min_width = isize->min_height = 2;
		isize->max_width = isize->max_height = 200;
		isize->width_inc = isize->height_inc = 1;

		XSetIconSizes(QX11Info::display(), QX11Info::appRootWindow(), isize, 1);
		XFree(isize);
	}	

        /*
	if(defaults::root_pix.isNull()) 
		a.desktop()->setPalette(QPalette(defaults::root_bg));
	else
	{
		QPalette br;
		br.setBrush(QPalette::Window, QBrush(QPixmap(defaults::root_pix)));
		a.desktop()->setPalette(br);
        }
        */
	XClearWindow(QX11Info::display(), QX11Info::appRootWindow());


	qapp::upal = new QPalette(defaults::urgent_bg);

	if(defaults::inactive_bg == NULL)
	{
		qapp::ipal = new QPalette;
		qapp::ipal->setBrush(QPalette::Window, qapp::ipal->brush(QPalette::Dark));
	}
	else
		qapp::ipal = new QPalette(*defaults::inactive_bg);

	XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), SubstructureRedirectMask);
	XSync(QX11Info::display(), FALSE);

	if(defaults::sttiled[0])
	{
		qapp::tdesks[0] = TRUE;
		defaults::sttiled[0] = FALSE;
	}
        HomeDesktop::instance()->getWinlist()->set_pixmap();
	getrunprocs();
	defaults::start_restart = FALSE;

        XSelectInput(QX11Info::display(), QX11Info::appRootWindow(),
	SubstructureNotifyMask|SubstructureRedirectMask| 
	StructureNotifyMask|
	ButtonPressMask|
	PropertyChangeMask|
	KeyPressMask|
	ColormapChangeMask|
	EnterWindowMask);

	XSync(QX11Info::display(), FALSE);

	struct sigaction act;
	
	act.sa_handler = sig_hup;
	act.sa_flags = SA_RESTART;
	sigaction(SIGHUP, &act, NULL);

	act.sa_handler = sig_term;
	sigaction(SIGTERM, &act, NULL);

	keyboard::init();
	wm_init = FALSE;
	startprg();
	
	return(a.exec());
}
