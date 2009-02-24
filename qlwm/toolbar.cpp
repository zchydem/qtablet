/*
*  File     : toolbar.cpp
*  Writen by: alinden@gmx.de
*  Copyright: GPL
*
*  Draw the toolbar and place items on it
*/

#include "defs.h"
#include "qapp.h"
#include "toolbar.h"
#include "defaults.h"
#include "pager.h"
#include "winlist.h"
#include "menu.h"
#include "procbar.h"
#include "apbar.h"
#include <QtDebug>
#include <QFont>

HomeDesktop * HomeDesktop::instance()
{
    static HomeDesktop instance;
    return &instance;
}

HomeDesktop::HomeDesktop( QWidget * parent ) :
        QWidget(parent )
{
    home = new qtablet::Home( this );
    home->setGeometry(0,0,800,480);
    toolbar = new Toolbar2(this);    
    toolbar->hide();
    setGeometry(0,0,800,480);
    show();

    if ( connect( home->pager(), SIGNAL(changeDesktop(int)), this, SLOT(changeVirtualDesktop(int)) ) ){
        qDebug() << "Connection success.::";
    }
}

pager   * HomeDesktop::getPager() const
{
    return toolbar->tb_pg;
}
winlist * HomeDesktop::getWinlist() const
{
    return toolbar->tb_wl;
}
menu    * HomeDesktop::getMenu() const
{
    return toolbar->tb_mn;
}
qtablet::Pager * HomeDesktop::getProcbar() const
{
    return home->pager();
    //return toolbar->tb_pb;
}
apbar   * HomeDesktop::getApbar() const
{
    return toolbar->tb_ap;
}

qtablet::Home * HomeDesktop::getHome() const
{
    return home;
}



void HomeDesktop::qButtonClicked(){
    qDebug() << "Clicked()";

    home->showOrHideWall();
}

void HomeDesktop::changeVirtualDesktop( int desktop ){
    xwindow *client;

    qDebug() << "HomeDesktop::changeVirtualDesktop to: " << desktop;
    if(desktop == qapp::adesk || desktop < 0 || desktop >= defaults::vdesks)
            return;

    int dwidth = QApplication::desktop()->width();
    int dx = (qapp::adesk-desktop)*dwidth;
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

    qapp::adesk = desktop;
    XUngrabServer(QX11Info::display());

    if(defaults::sttiled[desktop])
    {
            qapp::toggle_tiled();
            defaults::sttiled[desktop] = FALSE;
    }

    getProcbar()->setActiveDesktop( desktop );
}

/*
Toolbar * Desktop::toolbar() const{
    return toolbar;
}
*/
Toolbar2::Toolbar2(QWidget *parent) : QFrame(parent)
{

    setFrameStyle( QFrame::NoFrame );
        layout = new QVBoxLayout(0);
        layout->setSpacing(0);
        layout->setContentsMargins(0,0,0,0);
        QPushButton * button = new QPushButton("Q", this );
        button->setMinimumSize( 80, 80 );

        button->setFont( QFont("Arial", 24, 50 ) );
        button->setAutoFillBackground( true );
        QPalette palette = button->palette();
        QColor color = palette.color( QPalette::Window);
        color.setAlpha( 180 );
        palette.setColor( QPalette::Window, color );
        button->setPalette( palette );
        button->setWindowOpacity( 0.5 );

        connect( button, SIGNAL( clicked() ), this, SIGNAL(qButtonClicked()) );
        layout->addWidget( button );

	layout->setSpacing(1);
	
        qreal width  = QApplication::desktop()->width();
        qreal height = QApplication::desktop()->height();
        //setGeometry(0, 0, width, height );
        /*
	setGeometry(0, (defaults::toolbar_top)?(0):(QApplication::desktop()->height()-defaults::tb_height),
	QApplication::desktop()->width(), defaults::tb_height);
        */
	layout->addSpacing(4);



	// winlist
	if(defaults::show_winlist)
	{
		tb_wl = new winlist(this);
		layout->addWidget(tb_wl);
	}
	else
		tb_wl = new winlist(0);

	// menu
	if(defaults::show_menu)
	{
		tb_mn = new menu(this);
		layout->addWidget(tb_mn);
	}	
	else
		tb_mn = new menu(0);
		
	tb_mn->readmenu();

	if(defaults::show_menu || defaults::show_winlist)
		addsep();

	// procbar
	tb_pb = new procbar(this);
	tb_pb->setFixedHeight(defaults::tc_height);
	layout->addWidget(tb_pb);

        tb_ap = new apbar(this);
	layout->addWidget(tb_ap);


        // pager
        tb_pg = new pager(this);
        layout->addWidget(tb_pg);
        layout->addSpacing(3);

        setLayout( layout );
	show();
}

void Toolbar2::addsep(void)
{
        layout->addSpacing(3);
	QFrame *frame = new QFrame(this);
	frame->setLineWidth(1);
	frame->setMidLineWidth(0);
        //frame->setFrameStyle(QFrame::VLine|QFrame::Sunken);
	frame->setFixedHeight(height()-4);
	layout->addWidget(frame);
        layout->addSpacing(3);
}
