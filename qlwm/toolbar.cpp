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
    // For some weird reason we must call desktop()->width/height methods
    // in order to make some things to work like client window size.
    // TODO: Figure out where the problem is
    qreal widht = QApplication::desktop()->width();
    qreal height = QApplication::desktop()->height();
    Q_UNUSED( widht );
    Q_UNUSED( height );


    home = new qtablet::Home( this );
    home->setGeometry(0,0,800,480);    
    setGeometry(0,0,800,480);
    show();

    connect( home->pager(), SIGNAL(changeDesktop(int)), this, SLOT(changeVirtualDesktop(int)) );

}

qtablet::Pager   * HomeDesktop::getPager() const
{    
    return home->pager();
}

/*
qtablet::Pager * HomeDesktop::getProcbar() const
{
    return home->pager();    
}
*/
qtablet::Home * HomeDesktop::getHome() const
{
    return home;
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

    getPager()->setActiveDesktop( desktop );
}

