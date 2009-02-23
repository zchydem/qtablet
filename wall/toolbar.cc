#include "toolbar.hh"
#include "toolbarbutton.hh"

namespace qtablet{

Toolbar::Toolbar( QGraphicsItem * parent ):
    AbstractItem( parent )
{
    // TODO: make this use layouts
    ToolbarButton * qlButton = new ToolbarButton( ToolbarButton::QLauncherButton, this );
    qlButton->setPos(0,0);
    ToolbarButton * pButton = new ToolbarButton( ToolbarButton::QPagerButton, this );
    pButton->setPos(0,100);


    connect( qlButton, SIGNAL(clicked()), this, SIGNAL( qlauncherButtonClicked() ) );
    connect( pButton, SIGNAL(clicked()),  this, SIGNAL( pagerButtonClicked() ) );
}

Toolbar::~Toolbar(){

}

Pager * Toolbar::pager() const{
    return 0;
}

WindowList *  Toolbar::windowList() const{
    return 0;
}

ProcessBar * Toolbar::processBar() const{
    return 0;
}

ApplicationBar * Toolbar::appicationBar() const{

}

QSizeF Toolbar::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return QSizeF( 90, 480 );
}

}
