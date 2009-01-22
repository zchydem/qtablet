#include "home.hh"
#include "qlauncher.hh"
#include "wall.hh"
#include "settings.hh"

// Animation stuff
#include <qstate.h>
#include <qstategroup.h>
#include <qtransition.h>
#include <qanimation.h>

#include <QGraphicsScene>
#include <QDir>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QtDebug>

namespace qtablet{

class HomePrivate{
    public:
    HomePrivate():
            m_view( 0 ),
            m_scene( 0 ),
            m_state1( 0 ),
            m_state2( 0 ){}

    QGraphicsView  * m_view;
    QGraphicsScene * m_scene;
    QtState        * m_state1;
    QtState        * m_state2;
};

Home::Home( QWidget * parent ):
QGraphicsView( parent ),
d_ptr( new HomePrivate )
{
    QDir::setSearchPaths("config", QStringList( "../config") );
    qtablet::Settings * settings = qtablet::Settings::instance();
    Q_UNUSED( settings );

    d_ptr->m_scene = new QGraphicsScene(0, 0, 800, 480 );
    setScene( d_ptr->m_scene );
    resize( 800, 480 );
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setOptimizationFlag( QGraphicsView::DontClipPainter );
    setOptimizationFlag( QGraphicsView::DontSavePainterState );
    setFrameStyle( QFrame::NoFrame );

    qtablet::QLauncher * launcher = new qtablet::QLauncher;
    d_ptr->m_scene->addItem( launcher );

    qtablet::Wall * wall = new qtablet::Wall;
    d_ptr->m_scene->addItem( wall );

    launcher->setCacheMode( QGraphicsItem::ItemCoordinateCache );
    wall->setCacheMode( QGraphicsItem::ItemCoordinateCache );

    QPushButton * button1 = new QPushButton;
    button1->resize( 80, 480 );
    button1->setMinimumSize( 80, 480);
    button1->setMaximumSize( 80, 480);

    d_ptr->m_state1 = new QtState( d_ptr->m_scene );
    d_ptr->m_state2 = new QtState( d_ptr->m_scene );
    QtStateGroup * group = new QtStateGroup( d_ptr->m_scene );
    group->addState(d_ptr->m_state1);
    group->addState(d_ptr->m_state2);

    connect( launcher, SIGNAL(hideQLauncher()), d_ptr->m_state1, SLOT(activate()) );


    d_ptr->m_state1->setGeometry( wall,     QRectF(0,0,800,480) );    
    d_ptr->m_state1->setGeometry( launcher, QRectF(800,0,800,480) );
    d_ptr->m_state2->setGeometry( wall,     QRectF(-800,0,800,480) );
    d_ptr->m_state2->setGeometry( launcher, QRectF(0,0,800,480) );

    QtTransition * transition1 = new QtTransition(d_ptr->m_state1, d_ptr->m_state2, this );
    QtAnimation * anim1 = new QtAnimation(wall, "geometry");
    QtAnimation * anim2 = new QtAnimation(launcher, "geometry");

    anim1->setEasingCurve( QtEasingCurve::InOutCubic );
    anim1->setDuration( 1000 );
    anim2->setEasingCurve( QtEasingCurve::InOutCubic );
    anim2->setDuration( 1000 );

    transition1->add( anim1 );
    transition1->add( anim2 );

    QtTransition * transition2 = new QtTransition(d_ptr->m_state2, d_ptr->m_state1, this);
    QtAnimation * anim3 = new QtAnimation(wall, "geometry");
    QtAnimation * anim4 = new QtAnimation(launcher, "geometry");

    anim3->setEasingCurve( QtEasingCurve::OutQuad );
    anim3->setDuration( 1000 );
    anim4->setEasingCurve( QtEasingCurve::OutQuad );
    anim4->setDuration( 1000 );

    transition2->add(anim3);    
    transition2->add(anim4);

    d_ptr->m_state1->activate();
}


void Home::showOrHideWall(){
    if ( d_ptr->m_state1->isActive() ){
        d_ptr->m_state2->activate();
    }else{
        d_ptr->m_state1->activate();
    }
}
}
