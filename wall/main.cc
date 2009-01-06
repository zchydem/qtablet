#include "pannableview.hh"
#include "pannableviewitem.hh"
#include "settings.hh"
#include "qlauncher.hh"

#include <QApplication>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QDir>
#include <QPushButton>
#include <QImage>
#include <QtDebug>



// Animation stuff
#include <qstate.h>
#include <qstategroup.h>
#include <qtransition.h>
#include <qanimation.h>



QGraphicsLayout * createWall(){

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

  return layout;
}



int main( int argc, char *argv[] ){
  
  QApplication app( argc, argv );

  // Set first search path manually in order to make possible
  // to find the configuration file.
  QDir::setSearchPaths("config", QStringList( "../config") );
  qtablet::Settings * settings = qtablet::Settings::instance();
  Q_UNUSED( settings );


  QGraphicsScene scene(0,0,800,480);

  qtablet::QLauncher * launcher = new qtablet::QLauncher;  
  scene.addItem( launcher );

  qtablet::PannableView * wall = new qtablet::PannableView( Qt::Vertical, 800, 480 );
  wall->setLayout( createWall() );
  scene.addItem( wall );


  QPushButton * button1 = new QPushButton;
  button1->resize( 80, 480 );
  button1->setMinimumSize( 80, 480);
  button1->setMaximumSize( 80, 480);

  QGraphicsProxyWidget * button = new QGraphicsProxyWidget;
  button->setWidget( button1 );
  button->setOpacity( 0.5 );
  button->setZValue( 1 );
  scene.addItem( button );

  QtState * state1 = new QtState( &scene );
  QtState * state2 = new QtState( &scene );
  QtStateGroup group;
  group.addState(state1);
  group.addState(state2);

  state1->setConnectionState(button1, SIGNAL(clicked()), state2, SLOT(activate()));
  state1->setGeometry( wall,     QRectF(0,0,800,480) );
  state1->setGeometry( button,   QRectF(720,0, 80, 480 ) );
  state1->setGeometry( launcher, QRectF(800,0,800,480) );

  state2->setConnectionState(button1, SIGNAL(clicked()), state1, SLOT(activate()));
  state2->setGeometry( wall,     QRectF(-800,0,800,480) );
  state2->setGeometry( button,   QRectF(720,0, 80, 480 ) );
  state2->setGeometry( launcher, QRectF(0,0,800,480) );

  QtTransition transition1(state1, state2);
  QtAnimation * anim1 = new QtAnimation(wall, "geometry");
  QtAnimation * anim2 = new QtAnimation(launcher, "geometry");

  //anim1->setEasingCurve( QtEasingCurve::OutBounce );
  //anim1->setDuration( 1000 );
  //anim2->setEasingCurve( QtEasingCurve::OutBounce );
  //anim2->setDuration( 1000 );

  transition1.add( anim1 );
  transition1.add(new QtAnimation(button,   "geometry"));
  transition1.add( anim2 );

  QtTransition transition2(state2, state1);
  QtAnimation * anim3 = new QtAnimation(wall, "geometry");
  QtAnimation * anim4 = new QtAnimation(launcher, "geometry");

  //anim3->setEasingCurve( QtEasingCurve::OutBounce );
  anim3->setDuration( 1000 );
  //anim4->setEasingCurve( QtEasingCurve::OutBounce );
  anim4->setDuration( 1000 );

  transition2.add(anim3);
  transition2.add(new QtAnimation(button,   "geometry"));
  transition2.add(anim4);

  state1->activate();

  QGraphicsView view(&scene);
  view.resize( 800, 480 );
  view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  view.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  view.setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  view.setOptimizationFlag( QGraphicsView::DontClipPainter );
  view.setOptimizationFlag( QGraphicsView::DontSavePainterState );

  view.setFrameStyle( QFrame::NoFrame );

  QStringList args = app.arguments();
  if ( args.contains("-fullscreen") ){
    app.setOverrideCursor(Qt::BlankCursor);
    view.showFullScreen();
  }else{
    view.show();
  }

  return app.exec();
}
