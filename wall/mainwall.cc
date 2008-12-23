#include "mainwall.hh"

#include "pannableview.hh"
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QDir>
#include <QPushButton>
#include <QImage>
#include "qlauncher.hh"
#include <QTimer>

// Animation stuff
#include <qstate.h>
#include <qstategroup.h>
#include <qtransition.h>
#include <qanimation.h>



namespace qtablet{

  MainWall::MainWall( QWidget * parent ):
    QWidget( parent )
  {
    
    // Create The Wall
  QGraphicsProxyWidget *wallPaper1 = new QGraphicsProxyWidget;
  QGraphicsProxyWidget *wallPaper2 = new QGraphicsProxyWidget;
  QGraphicsProxyWidget *wallPaper3 = new QGraphicsProxyWidget;
  QGraphicsProxyWidget *wallPaper4 = new QGraphicsProxyWidget;

  QLabel * wp1 = new QLabel;
  QLabel * wp2 = new QLabel;
  QLabel * wp3 = new QLabel;
  QLabel * wp4 = new QLabel;

  wp1->setScaledContents( true );
  wp2->setScaledContents( true );
  wp3->setScaledContents( true );
  wp4->setScaledContents( true );
  wp1->resize(800,480);
  wp2->resize(800,480);
  wp3->resize(800,480);
  wp4->resize(800,480);

  wp1->setPixmap(QPixmap("/opt/qtablet/images/bg_1.png"));
  wp2->setPixmap(QPixmap("/opt/qtablet/images/bg_2.png"));
  wp3->setPixmap(QPixmap("/opt/qtablet/images/bg_3.png"));
  wp4->setPixmap(QPixmap("/opt/qtablet/images/bg_4.png"));
  
  wallPaper1->setWidget( wp1 );
  wallPaper2->setWidget( wp2 );
  wallPaper3->setWidget( wp3 );
  wallPaper4->setWidget( wp4 );

  QGraphicsLinearLayout * layout   = new QGraphicsLinearLayout( Qt::Vertical );
  layout->setSpacing(0);

  layout->addItem( wallPaper1 );
  layout->addItem( wallPaper2 );
  layout->addItem( wallPaper3 );
  layout->addItem( wallPaper4 );

  QGraphicsWidget * wall = new QGraphicsWidget;
  wall->setLayout( layout );

  // Create the Launcher
  QLauncher * launcher = new QLauncher;
  

  QGraphicsLinearLayout * main = new QGraphicsLinearLayout( Qt::Horizontal );
  main->addItem( wall );
  main->addItem( launcher );
  QGraphicsWidget * widget = new QGraphicsWidget;
  widget->setLayout( main );


  QGraphicsScene scene;
  scene.addItem( widget );
  QGraphicsView window( &scene );
  window.resize( 800, 480 );
  window.setFrameStyle(0);
  //window.setAlignment(Qt::AlignLeft | Qt::AlignTop);
  window.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  window.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  /*
  QtStateGroup group;
  state1 = new QtState(&group);
  state2 = new QtState(&group);
  state1->setGeometry(wall, QRect(0, 0, 750, 480));
  state1->setGeometry(launcher, QRect(750, 0, 800, 480));
  state2->setGeometry(wall, QRect(-800, 0, 750, 480));
  state2->setGeometry(launcher, QRect(0, 0, 800, 480));

  QtTransition transition(0,0);
  anim1 = new QtAnimation(wall, "geometry");
  anim1->setDuration(1000);
  anim1->setEasingCurve(QtEasingCurve::OutElastic);
  transition.add(anim1);
  //transition.beginSequentialGroup();
  //transition.addPause(100);
  anim2 = new QtAnimation(launcher, "geometry");
  anim2->setDuration(1000);
  anim2->setEasingCurve(QtEasingCurve::OutElastic);
  transition.add(anim2);

  QTimer::singleShot( 10000, this, SLOT( animate() ) );
  */
  }

  void MainWall::animate(){
    state2->activate();
  }
  
  MainWall::~MainWall(){}
}


