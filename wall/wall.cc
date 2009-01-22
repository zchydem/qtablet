#include "wall.hh"
#include "pannableviewitem.hh"
#include "pannableview.hh"

#include <QGraphicsLinearLayout>



namespace qtablet{

Wall::Wall( QGraphicsItem * parent ):
AbstractItem( parent )
{
    // TODO: Replace this harcoded stuff with real implementation
    QGraphicsLinearLayout * layout   = new QGraphicsLinearLayout( Qt::Vertical );
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    qtablet::PannableViewItem * d1 = new qtablet::PannableViewItem( "image:default_1.png" );
    qtablet::PannableViewItem * d2 = new qtablet::PannableViewItem( "image:default_2.png" );
    qtablet::PannableViewItem * d3 = new qtablet::PannableViewItem( "image:default_3.png" );
    qtablet::PannableViewItem * d4 = new qtablet::PannableViewItem( "image:default_4.png" );

    d1->setShowSelection( false );
    d2->setShowSelection( false );
    d3->setShowSelection( false );
    d4->setShowSelection( false );

    d1->setAcceptMouseEvent( false );
    d2->setAcceptMouseEvent( false );
    d3->setAcceptMouseEvent( false );
    d4->setAcceptMouseEvent( false );

    layout->addItem( d1 );
    layout->addItem( d2 );
    layout->addItem( d3 );
    layout->addItem( d4 );

    qtablet::PannableView * wall = new qtablet::PannableView( Qt::Vertical, 800, 480 );
    wall->setLayout( layout );

    QGraphicsLinearLayout * mainLayout = new QGraphicsLinearLayout( Qt::Vertical );
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addItem( wall );
    setLayout( mainLayout );
}

QSizeF Wall::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return QSizeF(800,480);
}

}
