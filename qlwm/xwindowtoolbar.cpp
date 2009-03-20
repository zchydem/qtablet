#include "xwindowtoolbar.h"
#include "toolbarbutton.hh"
#include "labelitem.hh"

#include <QPen>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>

class XWindowToolbarPrivate{
public:
    qtablet::ToolbarButton *m_minimizeButton;
    qtablet::ToolbarButton *m_menuButton;
    qtablet::ToolbarButton *m_closeButton;
    qtablet::LabelItem     *m_title;
};


XWindowToolbar::XWindowToolbar( bool transient, QGraphicsItem * parent ):
        QGraphicsWidget( parent ),
        d_ptr( new XWindowToolbarPrivate )
{
    qint32 width   = qApp->desktop()->width();
    qint32 height  = 85;
    qint32 xOffset = 5;

    if ( !transient ){
        QGraphicsRectItem * item = new QGraphicsRectItem(0,0,width, height, this);
        QColor color( Qt::lightGray );
        color.setAlphaF(0.5);
        item->setBrush( color );
        item->setPen( QPen(Qt::black, 3) );
        item->setPos(QPointF(0,0) );
        item->setZValue(0);
    }

    QSize buttonSize( height - 20 , height - 20  );

    d_ptr->m_minimizeButton = new qtablet::ToolbarButton( qtablet::ToolbarButton::MinimizeButton, this );
    d_ptr->m_menuButton     = new qtablet::ToolbarButton( qtablet::ToolbarButton::MenuButton, this );
    d_ptr->m_closeButton    = new qtablet::ToolbarButton( qtablet::ToolbarButton::CloseButton, this );
    d_ptr->m_title          = new qtablet::LabelItem( this );

    d_ptr->m_minimizeButton->setVisible( !transient );
    d_ptr->m_closeButton->setVisible( !transient );
    d_ptr->m_title->setVisible( !transient );

    d_ptr->m_minimizeButton->resize( buttonSize );
    d_ptr->m_menuButton->resize( buttonSize );
    d_ptr->m_closeButton->resize( buttonSize );

    qint32 titleWidth = width - ( xOffset*3 + buttonSize.width()*3);
    d_ptr->m_title->setWidth(titleWidth);    
    d_ptr->m_title->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    d_ptr->m_title->setFont( QFont("Arial", 20, 55 ) );


    qint32 yPos = ( height - buttonSize.height() ) / 2;

    d_ptr->m_menuButton->setPos( xOffset, yPos );
    d_ptr->m_minimizeButton->setPos( width - buttonSize.width()*2 - xOffset*2, yPos );
    d_ptr->m_closeButton->setPos( width - buttonSize.width() - xOffset, yPos);
    d_ptr->m_title->setPos( xOffset*2 + buttonSize.width(), 0 );

    d_ptr->m_menuButton->setZValue( 1 );
    d_ptr->m_minimizeButton->setZValue(1);
    d_ptr->m_closeButton->setZValue(1);
    d_ptr->m_title->setZValue(1);

    connect( d_ptr->m_minimizeButton, SIGNAL(clicked()), this, SIGNAL( minimizeWindow() ) );
    connect( d_ptr->m_menuButton,     SIGNAL(clicked()), this, SIGNAL( showMenu() ) );
    connect( d_ptr->m_closeButton,    SIGNAL(clicked()), this, SIGNAL( closeWindow() ) );

}

XWindowToolbar::~XWindowToolbar(){

}

void XWindowToolbar::setTitle( const QString & title ){
    d_ptr->m_title->setText(title);
}
