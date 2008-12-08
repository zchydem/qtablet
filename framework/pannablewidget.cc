#include "pannablewidget.hh"
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QTime>
#include <QPoint>
#include <QTimeLine>
#include <QtDebug>
#include <math.h>

class PannableWidgetPrivate{
public:
  PannableWidgetPrivate():
    m_acceleration( 0 ),
    m_deltaX( 0 ),
    m_time(),
    m_scrollingTimeLine( 0 ),
    m_mousePressPos(),
    m_mouseReleasePos(),
    m_lastMovePos(),
    m_isScrollingEnabled( false ),
    m_isPressed( false )
   {}

  // Members
  qreal       m_acceleration;
  qreal       m_deltaX;
  QTime       m_time;
  QTimeLine * m_scrollingTimeLine;
  QPoint      m_mousePressPos;
  QPoint      m_mouseReleasePos;
  QPoint      m_lastMovePos;
  bool        m_isScrollingEnabled;
  bool        m_isPressed;
};

namespace qtablet{

  PannableWidget::PannableWidget( QWidget * parent ):
    QGraphicsView( parent ),
    d_ptr( new PannableWidgetPrivate )
  {
    setFrameStyle( QFrame::NoFrame );
    setOptimizationFlag( QGraphicsView::DontAdjustForAntialiasing, false );
    setOptimizationFlag( QGraphicsView::DontClipPainter, false );
    setOptimizationFlag( QGraphicsView::DontSavePainterState,true );
    setMouseTracking( true );

        
    setInteractive( true );
    setBackgroundBrush( Qt::black );
    setTransformationAnchor( QGraphicsView::NoAnchor );
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    

    d_ptr->m_scrollingTimeLine = new QTimeLine( 1000, this );
    d_ptr->m_scrollingTimeLine->setCurveShape( QTimeLine::EaseInCurve );
    d_ptr->m_scrollingTimeLine->setDirection( QTimeLine::Backward );
        
    connect( d_ptr->m_scrollingTimeLine, SIGNAL( valueChanged( qreal ) ),
	     this,                       SLOT  ( scroll( qreal ) ) );


  }
  
  PannableWidget::~PannableWidget(){

  }

  void PannableWidget::setPannableWidget( QGraphicsWidget * widget ){
    
    if ( 0 == widget ){
      return;
    }

    QGraphicsScene * scene = new QGraphicsScene;
    QSizeF size = widget->effectiveSizeHint( Qt::MinimumSize );
    scene->setSceneRect( 0,0, size.width(), size.height() );
    setScene( scene );
    scene->addItem( widget );
  }

  void PannableWidget::mousePressEvent( QMouseEvent * event ){
    //qDebug() << "Pressed";
    /*
    if ( !d_ptr->m_isScrollingEnabled ){
      QGraphicsView::mousePressEvent( event );
      return;
    }
    */      
        
    d_ptr->m_isPressed   = true;
    d_ptr->m_lastMovePos = event->pos();
    d_ptr->m_scrollingTimeLine->stop();
    d_ptr->m_mousePressPos  = event->pos();
    d_ptr->m_time.start();        
  }

  void PannableWidget::mouseReleaseEvent( QMouseEvent * event ){
    /*
    if ( !d_ptr->m_isScrollingEnabled ){
      QGraphicsView::mouseReleaseEvent( event );
      return;
    }
    */  
    d_ptr->m_isPressed = false;

    qreal  dD       = (event->pos() - d_ptr->m_mousePressPos).x(); // Delta X distance
    qint32 tD       =  d_ptr->m_time.elapsed() ;                // Delta Time
    d_ptr->m_acceleration  = fabs(dD/tD);                       // Acceleration
        

    qint32 multiplier = 1000;
        
    if ( fabs(dD) > 20 ){
      //qDebug() << "dD: " << dD << "tD: " << tD << "a:" << d_ptr->m_acceleration ;
      d_ptr->m_deltaX = dD;

      //if ( d_ptr->m_acceleration <= 0.2 ) multiplier = 100;
      if ( d_ptr->m_acceleration <= 0.4 ) multiplier = 0;
      else if ( d_ptr->m_acceleration <= 0.6 ) multiplier = 200;
      else if ( d_ptr->m_acceleration <= 0.8 ) multiplier = 300;
      else if ( d_ptr->m_acceleration <= 1.0 ) multiplier = 400;
      else if ( d_ptr->m_acceleration <= 1.2 ) multiplier = 500;
      else if ( d_ptr->m_acceleration <= 1.4 ) multiplier = 600;
      else if ( d_ptr->m_acceleration <= 1.6 ) multiplier = 700;
      else if ( d_ptr->m_acceleration <= 1.8 ) multiplier = 800;
      else if ( d_ptr->m_acceleration <= 2.0 ) multiplier = 900;
            
      // User has done scrolling movement. Let's set correct duration
      // for the scrolling based on the calculated acceleration and
      // start a new animation for scrolling.
      //d_ptr->m_scrollingTimeLine->blockSignals( false );
      if ( multiplier != 0 ){
	qDebug() << "Starting  to scroll...";
	d_ptr->m_scrollingTimeLine->setDuration(  static_cast<qint32>( d_ptr->m_acceleration * multiplier ) );
	d_ptr->m_scrollingTimeLine->start();
      }
      return;
    }
    
    // If endup here user has only clicked the the display
    // Get item from the clicked position
    
    //QGraphicsItem * item = d_ptr->m_scene->itemAt( mapToScene( event->pos() ) );

  }

  void PannableWidget::mouseMoveEvent(  QMouseEvent * event ){
    /*
    if ( !d_ptr->m_isScrollingEnabled ){
      QGraphicsView::mouseMoveEvent( event );
      return;
    }
    */
    if ( d_ptr->m_isPressed ){
	
	qreal delta = ( event->pos()  - d_ptr->m_lastMovePos ).x();
	
	// Store this position for the next event            
	d_ptr->m_lastMovePos = event->pos();
	
	// Filter out too smale movements
        
	if ( fabs( delta ) == 0 ){
	  return;
	}
        
	// translate the view by delta
	//if ( d_ptr->m_isScrollingEnabled ){
	  translate( delta, 0 );
	  //}
	
      }
  }

  void PannableWidget::scroll( qreal value ){
    // Calculate the start and end positions
    // based on the current transformation matrix
    // and the width of the scene
    QTransform xform = viewportTransform();

    //qDebug() << d_ptr->m_deltaX;
    if ( value == 0 ){
      d_ptr->m_acceleration = 0;
      return;
    }
    
    //qDebug() << xform;
    if ( fabs(xform.m31()) < 2 ){    
      d_ptr->m_scrollingTimeLine->stop();
      return;
    }else
    if ( ( fabs(xform.m31()) + 800 ) >= scene()->sceneRect().width() ){	
      d_ptr->m_scrollingTimeLine->stop();
      return;
    }
   
    
    // Scroll to the left or right
    if ( d_ptr->m_deltaX > 0 ){
      translate( value*100, 0 );
    }else
      if ( d_ptr->m_deltaX < 0 ){
	translate( value*-100, 0 );
      }
  }

}
