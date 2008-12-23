#include "qlauncher.hh"
#include "pannableview.hh"
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QGraphicsWidget>
#include <QPushButton>
#include <QDir>

namespace qtablet{

  QLauncher::QLauncher( QGraphicsWidget * parent ):
    QGraphicsWidget( parent ){
    
    

    QGraphicsGridLayout * layout = new QGraphicsGridLayout;
    layout->setSpacing( 20 );
    int row = 0;
    int i   = 0;
    QDir dir("/opt/img/thumbs");
    QStringList thumbs = dir.entryList( QDir::Files );
    

    
    foreach( QString thumb, thumbs ){
      //for ( int j=0; j<300; j++ ){
      QGraphicsProxyWidget *w1 = new QGraphicsProxyWidget;
      QLabel * l1 = new QLabel;
      l1->setPixmap( QPixmap("/opt/img/thumbs/"+thumb ) );    
      w1->setWidget( l1 );
      layout->addItem( w1,   row++%3, i);
      
      if ( row % 3 == 0 ){
	++i;
      }
    }
    
  
    QGraphicsWidget * launcher = new QGraphicsWidget;
    launcher->setLayout( layout );    
    pannableView = new PannableView( Qt::Horizontal );
    pannableView->setPannableWidget( launcher );

    // Main Layout
    QGraphicsLinearLayout * layout2 = new QGraphicsLinearLayout;
    QGraphicsProxyWidget * button  = new QGraphicsProxyWidget;
    QGraphicsProxyWidget * menu    = new QGraphicsProxyWidget;

    QPushButton *bar = new QPushButton;
    bar->resize( 80, 480 );
    pannableView->resize( 720, 480 );
    button->setWidget( bar );
    menu->setWidget( pannableView );
    
    layout2->addItem( button );
    layout2->addItem( menu );

    setLayout( layout2 );
  }

  QLauncher::~QLauncher(){

  }

};
