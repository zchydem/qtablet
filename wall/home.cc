#include "home.hh"
#include "qlauncher.hh"
#include "wall.hh"
#include "settings.hh"
#include "toolbar.hh"
#include "pager.hh"

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
            m_view  ( 0 ),
            m_scene ( 0 ),
            m_state1( 0 ),
            m_state2( 0 ),
            m_state3( 0 ),
            m_pager ( 0 )
    {}

    void createTransition( QtState * from, QtState * to, QObject * parent ){
        if ( m_wall == 0 && m_launcher == 0 && m_pager == 0 ){
            qCritical() << "Home: Can't create transition for NULL objects";
            return;
        }

        QtTransition * transition = new QtTransition( from, to, parent );
        QtAnimation * anim1       = new QtAnimation( m_wall,     "geometry");
        QtAnimation * anim2       = new QtAnimation( m_launcher, "geometry");
        QtAnimation * anim3       = new QtAnimation( m_pager,    "geometry");

        qint32 duration = 1000;
        anim1->setEasingCurve( QtEasingCurve::InOutCubic );
        anim1->setDuration( duration );
        anim2->setEasingCurve( QtEasingCurve::InOutCubic );
        anim2->setDuration( duration );
        anim3->setEasingCurve( QtEasingCurve::InOutCubic );
        anim3->setDuration( duration );

        transition->add( anim1 );
        transition->add( anim2 );
        transition->add( anim3 );
    }

    QGraphicsView  * m_view;
    QGraphicsScene * m_scene;
    QtState        * m_state1;
    QtState        * m_state2;
    QtState        * m_state3;    
    Pager          * m_pager;
    Wall           * m_wall;
    Toolbar        * m_toolbar;
    QLauncher      * m_launcher;
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

    d_ptr->m_launcher = new (std::nothrow )QLauncher;
    Q_CHECK_PTR( d_ptr->m_launcher );
    d_ptr->m_scene->addItem( d_ptr->m_launcher );

    d_ptr->m_wall = new (std::nothrow) Wall;
    Q_CHECK_PTR( d_ptr->m_wall );
    d_ptr->m_scene->addItem( d_ptr->m_wall );

    d_ptr->m_toolbar = new (std::nothrow) Toolbar;
    Q_CHECK_PTR( d_ptr->m_toolbar );
    d_ptr->m_scene->addItem( d_ptr->m_toolbar );
    d_ptr->m_toolbar->setZValue( 2 );

    d_ptr->m_pager = new (std::nothrow) Pager;
    Q_CHECK_PTR( d_ptr->m_pager );
    d_ptr->m_scene->addItem( d_ptr->m_pager );


    //d_ptr->m_launcher->setCacheMode( QGraphicsItem::ItemCoordinateCache );
    //d_ptr->m_wall->setCacheMode( QGraphicsItem::ItemCoordinateCache );


    d_ptr->m_state1 = new QtState( d_ptr->m_scene );
    d_ptr->m_state2 = new QtState( d_ptr->m_scene );
    d_ptr->m_state3 = new QtState( d_ptr->m_scene );

    QtStateGroup * group = new QtStateGroup( d_ptr->m_scene );
    group->addState(d_ptr->m_state1);
    group->addState(d_ptr->m_state2);
    group->addState(d_ptr->m_state3);

    connect( d_ptr->m_launcher, SIGNAL(hideQLauncher()),          this, SLOT( showOrHideWall () ) );
    connect( d_ptr->m_toolbar,  SIGNAL(pagerButtonClicked()),     this, SLOT( showOrHidePager() ) );
    connect( d_ptr->m_toolbar,  SIGNAL(qlauncherButtonClicked()), this, SLOT( showOrHideWall () ) );

    // Wall is visible
    d_ptr->m_state1->setGeometry( d_ptr->m_toolbar,  QRectF(710,0,90,480 ) );
    d_ptr->m_state1->setGeometry( d_ptr->m_wall,     QRectF(0,0,800,480) );
    d_ptr->m_state1->setGeometry( d_ptr->m_launcher, QRectF(800,0,800,480) );
    d_ptr->m_state1->setGeometry( d_ptr->m_pager,    QRectF(0,480,710,480) );

    // QLauncher is visible
    d_ptr->m_state2->setGeometry( d_ptr->m_toolbar,  QRectF(710,0,90,480 ) );
    d_ptr->m_state2->setGeometry( d_ptr->m_wall,     QRectF(-800,0,800,480) );
    d_ptr->m_state2->setGeometry( d_ptr->m_launcher, QRectF(0,0,800,480) );
    d_ptr->m_state2->setGeometry( d_ptr->m_pager,    QRectF(0,480,710,480) );

    //! Pager is visible
    d_ptr->m_state3->setGeometry( d_ptr->m_toolbar,  QRectF(710,0,90,480 ) );
    d_ptr->m_state3->setGeometry( d_ptr->m_wall,     QRectF(0, -480,800,480) );
    d_ptr->m_state3->setGeometry( d_ptr->m_launcher, QRectF(800,0,800,480) );
    d_ptr->m_state3->setGeometry( d_ptr->m_pager,    QRectF(0, 0,710,480) );


    d_ptr->createTransition( d_ptr->m_state1, d_ptr->m_state2, this );
    d_ptr->createTransition( d_ptr->m_state2, d_ptr->m_state1, this );
    d_ptr->createTransition( d_ptr->m_state1, d_ptr->m_state3, this );
    d_ptr->createTransition( d_ptr->m_state3, d_ptr->m_state1, this);
    d_ptr->createTransition( d_ptr->m_state2, d_ptr->m_state3, this );
    d_ptr->createTransition( d_ptr->m_state3, d_ptr->m_state2, this );

    /*
    // Transition from wall to qlauncher
    QtTransition * transition1 = new QtTransition(d_ptr->m_state1, d_ptr->m_state2, this );
    QtAnimation * anim1 = new QtAnimation(wall, "geometry");
    QtAnimation * anim2 = new QtAnimation(launcher, "geometry");
    QtAnimation * anim22 = new QtAnimation(d_ptr->m_pager, "geometry");

    anim1->setEasingCurve( QtEasingCurve::InOutCubic );
    anim1->setDuration( 1000 );
    anim2->setEasingCurve( QtEasingCurve::InOutCubic );
    anim2->setDuration( 1000 );
    anim22->setEasingCurve( QtEasingCurve::InOutCubic );
    anim22->setDuration( 1000 );

    transition1->add( anim1 );
    transition1->add( anim2 );
    transition1->add( anim22 );

    // Transition from qlauncher to wall
    QtTransition * transition2 = new QtTransition(d_ptr->m_state2, d_ptr->m_state1, this);
    QtAnimation * anim3 = new QtAnimation(wall, "geometry");
    QtAnimation * anim4 = new QtAnimation(launcher, "geometry");
    QtAnimation * anim44 = new QtAnimation(d_ptr->m_pager, "geometry");

    anim3->setEasingCurve( QtEasingCurve::OutQuad );
    anim3->setDuration( 1000 );
    anim4->setEasingCurve( QtEasingCurve::OutQuad );
    anim4->setDuration( 1000 );
    anim44->setEasingCurve( QtEasingCurve::OutQuad );
    anim44->setDuration( 1000 );

    transition2->add(anim3);
    transition2->add(anim4);
    transition2->add(anim44);


    // Transition from wall to pager
    QtTransition * transition3 = new QtTransition(d_ptr->m_state1, d_ptr->m_state3, this);
    QtAnimation * anim5 = new QtAnimation(wall, "geometry");
    QtAnimation * anim6 = new QtAnimation(d_ptr->m_pager, "geometry");
    QtAnimation * anim66 = new QtAnimation(launcher, "geometry");

    anim5->setEasingCurve( QtEasingCurve::OutQuad );
    anim5->setDuration( 1000 );
    anim6->setEasingCurve( QtEasingCurve::OutQuad );
    anim6->setDuration( 1000 );
    anim66->setEasingCurve( QtEasingCurve::OutQuad );
    anim66->setDuration( 1000 );

    transition3->add(anim5);
    transition3->add(anim6);
    transition3->add(anim66);

    // Transition from pager to wall
    QtTransition * transition4 = new QtTransition(d_ptr->m_state3, d_ptr->m_state1, this);
    QtAnimation * anim7 = new QtAnimation(wall, "geometry");
    QtAnimation * anim8 = new QtAnimation(d_ptr->m_pager, "geometry");
    QtAnimation * anim88 = new QtAnimation(launcher, "geometry");

    anim7->setEasingCurve( QtEasingCurve::OutQuad );
    anim7->setDuration( 1000 );
    anim8->setEasingCurve( QtEasingCurve::OutQuad );
    anim8->setDuration( 1000 );
    anim8->setEasingCurve( QtEasingCurve::OutQuad );
    anim8->setDuration( 1000 );

    transition4->add(anim7);
    transition4->add(anim8);
    transition4->add(anim88);

    QtTransition * transition5 = new QtTransition(d_ptr->m_state2, d_ptr->m_state3, this);
    QtAnimation * anim9 = new QtAnimation(wall, "geometry");
    QtAnimation * anim10 = new QtAnimation(d_ptr->m_pager, "geometry");
    QtAnimation * anim11 = new QtAnimation(launcher, "geometry");

    anim9->setEasingCurve( QtEasingCurve::OutQuad );
    anim9->setDuration( 1000 );
    anim10->setEasingCurve( QtEasingCurve::OutQuad );
    anim10->setDuration( 1000 );
    anim11->setEasingCurve( QtEasingCurve::OutQuad );
    anim11->setDuration( 1000 );

    transition5->add(anim9);
    transition5->add(anim10);
    transition5->add(anim11);

    QtTransition * transition6 = new QtTransition(d_ptr->m_state3, d_ptr->m_state2, this);
    QtAnimation * anim12 = new QtAnimation(wall, "geometry");
    QtAnimation * anim13 = new QtAnimation(d_ptr->m_pager, "geometry");
    QtAnimation * anim14 = new QtAnimation(launcher, "geometry");

    anim12->setEasingCurve( QtEasingCurve::OutQuad );
    anim12->setDuration( 1000 );
    anim13->setEasingCurve( QtEasingCurve::OutQuad );
    anim13->setDuration( 1000 );
    anim14->setEasingCurve( QtEasingCurve::OutQuad );
    anim14->setDuration( 1000 );

    transition6->add(anim12);
    transition6->add(anim13);
    transition6->add(anim14);
    */

    d_ptr->m_state1->activate();
}


void Home::showOrHideWall(){
    if ( d_ptr->m_state1->isActive() ){
        d_ptr->m_state2->activate();
        return;
    }
    if ( d_ptr->m_state2->isActive() ){
        d_ptr->m_state1->activate();
        return;
    }
    if ( d_ptr->m_state3->isActive() ){
        d_ptr->m_state2->activate();
        return;
    }
}

void Home::showOrHidePager(){
    if ( d_ptr->m_state3->isActive() ){
        d_ptr->m_state1->activate();
        return;
    }
    if ( d_ptr->m_state1->isActive() ){
        d_ptr->m_state3->activate();
        return;
    }
    if ( d_ptr->m_state2->isActive() ){        
        d_ptr->m_state3->activate();
        return;
    }

}

Pager * Home::pager() const{
    return d_ptr->m_pager;
}
}
