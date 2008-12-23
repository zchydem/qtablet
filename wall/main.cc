#include "pannableview.hh"
#include "imageitem.hh"

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
  layout->addItem( new qtablet::ImageItem( "../images/default_1.png" ) );
  layout->addItem( new qtablet::ImageItem( "../images/default_2.png" ) );
  layout->addItem( new qtablet::ImageItem( "..images/default_3.png" ) );
  layout->addItem( new qtablet::ImageItem( "../images/default_4.png" ) );
  return layout;
}

QGraphicsLayout * createLauncher(){
  
  QGraphicsGridLayout * layout = new QGraphicsGridLayout;
  layout->setSpacing( 20 );
  layout->setContentsMargins( 10, 10, 10, 10 );
  int row = 0;
  int i   = 0;
  QDir dir("../images/thumbs");
  QStringList thumbs = dir.entryList( QDir::Files );


    
  foreach( QString thumb, thumbs ){
  //for ( int j=0; j<300; j++ ){

    layout->addItem( new qtablet::ImageItem( "../images/thumbs/"+thumb) ,   row++%3, i);

    if ( row % 3 == 0 ){
      ++i;
    }
  }

  return layout;
}


QGraphicsWidget * createToolbar(){
  QPushButton * button = new QPushButton;
  button->resize( 50, 480 );
  button->setMinimumSize( 50, 480);
  button->setMaximumSize( 50, 480);

  QGraphicsProxyWidget * widget = new QGraphicsProxyWidget;
  widget->setWidget( button );
  return widget;

}

int main( int argc, char *argv[] ){
  
  QApplication app( argc, argv );
  QGraphicsScene scene(0,0,800,480);

  qtablet::PannableView * launcher = new qtablet::PannableView( Qt::Horizontal, 720, 480 );
  launcher->setLayout( createLauncher() );
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
  state1->setGeometry( launcher, QRectF(800,0,720,480) );
  state1->setOpacity( wall,      double(1) );
  state1->setOpacity( button,    double(1) );
  state1->setOpacity( launcher,  double(0) );

  state2->setConnectionState(button1, SIGNAL(clicked()), state1, SLOT(activate()));
  state2->setGeometry( wall,     QRectF(-800,0,800,480) );
  state2->setGeometry( button,   QRectF(720,0, 80, 480 ) );
  state2->setGeometry( launcher, QRectF(0,0,720,480) );
  state2->setOpacity( wall,      double(0) );
  state2->setOpacity( button,    double(1) );
  state2->setOpacity( launcher,  double(1) );

  QtTransition transition1(state1, state2);
  transition1.add(new QtAnimation(wall,     "geometry"));
  transition1.add(new QtAnimation(button,   "geometry"));
  transition1.add(new QtAnimation(launcher, "geometry"));
  transition1.add(new QtAnimation(wall,     "opacity"));
  transition1.add(new QtAnimation(launcher, "opacity"));

  QtTransition transition2(state2, state1);
  transition2.add(new QtAnimation(wall,     "geometry"));
  transition2.add(new QtAnimation(button,   "geometry"));
  transition2.add(new QtAnimation(launcher, "geometry"));
  transition2.add(new QtAnimation(wall,     "opacity"));
  transition2.add(new QtAnimation(launcher, "opacity"));

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