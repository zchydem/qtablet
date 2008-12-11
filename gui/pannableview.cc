#include "pannableview.hh"
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QTime>
#include <QPoint>
#include <QTimeLine>
#include <QtDebug>
#include <math.h>

class PannableViewPrivate{
public:
  PannableViewPrivate():
    m_acceleration( 0 ),
    m_deltaX( 0 ),
    m_deltaY( 0 ),
    m_time(),
    m_scrollingTimeLine( 0 ),
    m_mousePressPos(),
    m_mouseReleasePos(),
    m_lastMovePos(),
    m_isScrollingEnabled( false ),
    m_isPressed( false )
   {}

  // Members
  qreal           m_acceleration;
  qreal           m_deltaX;
  qreal           m_deltaY;
  QTime           m_time;
  QTimeLine     * m_scrollingTimeLine;
  QPoint          m_mousePressPos;
  QPoint          m_mouseReleasePos;
  QPoint          m_lastMovePos;
  bool            m_isScrollingEnabled;
  bool            m_isPressed;
  Qt::Orientation m_orientation;
};

namespace qtablet{

  PannableView::PannableView( Qt::Orientation orientation, QWidget * parent ):
    QGraphicsView( parent ),
    d_ptr( new PannableViewPrivate )
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
    
    d_ptr->m_orientation = orientation;
    d_ptr->m_scrollingTimeLine = new QTimeLine( 1000, this );
    d_ptr->m_scrollingTimeLine->setCurveShape( QTimeLine::EaseInCurve );
    d_ptr->m_scrollingTimeLine->setDirection( QTimeLine::Backward );
        
    connect( d_ptr->m_scrollingTimeLine, SIGNAL( valueChanged( qreal ) ),
	     this,                       SLOT  ( scroll( qreal ) ) );


  }
  
  PannableView::~PannableView(){

  }

  void PannableView::setPannableWidget( QGraphicsWidget * widget ){
    
    if ( 0 == widget ){
      return;
    }

    QGraphicsScene * scene = new QGraphicsScene;
    QSizeF size = widget->effectiveSizeHint( Qt::MinimumSize );
    scene->setSceneRect( 0,0, size.width(), size.height() );
    setScene( scene );
    scene->addItem( widget );
  }

  void PannableView::mousePressEvent( QMouseEvent * event ){

    QGraphicsView::mousePressEvent( event );

    d_ptr->m_isPressed   = true;
    d_ptr->m_lastMovePos = event->pos();
    d_ptr->m_scrollingTimeLine->stop();
    d_ptr->m_mousePressPos  = event->pos();
    d_ptr->m_time.start();        
  }

  void PannableView::mouseReleaseEvent( QMouseEvent * event ){

    d_ptr->m_isPressed = false;

    qreal  dxD             = (event->pos() - d_ptr->m_mousePressPos).x(); // Delta X distance
    qreal  dyD             = (event->pos() - d_ptr->m_mousePressPos).y(); // Delta Y distance
    qint32 tD              =  d_ptr->m_time.elapsed() ;                   // Delta Time

    // Calculate teh acceleration
    if ( d_ptr->m_orientation == Qt::Horizontal ){
      d_ptr->m_acceleration = fabs( dxD / tD ); 
    }else{
      d_ptr->m_acceleration = fabs( dyD / tD );
    }
        

    qint32 multiplier = 1000;
        
    if ( fabs(dxD) > 20 || fabs(dyD) > 20 ){
      //qDebug() << "dD: " << dD << "tD: " << tD << "a:" << d_ptr->m_acceleration ;
      d_ptr->m_deltaX = dxD;
      d_ptr->m_deltaY = dyD;

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
	d_ptr->m_scrollingTimeLine->setDuration(  static_cast<qint32>( d_ptr->m_acceleration * multiplier ) );
	d_ptr->m_scrollingTimeLine->start();
      }
      return;
    }
    
    // If endup here user has only clicked the the display
    // Get item from the clicked position    
    QGraphicsView::mouseReleaseEvent( event );

  }

  void PannableView::mouseMoveEvent(  QMouseEvent * event ){

    if ( d_ptr->m_isPressed ){
	
      qreal deltaX = 0;
      qreal deltaY = 0;
      
      if ( d_ptr->m_orientation == Qt::Horizontal ){
	deltaX = ( event->pos()  - d_ptr->m_lastMovePos ).x();
      }else{
	deltaY = ( event->pos()  - d_ptr->m_lastMovePos ).y();
      }
      // Store this position for the next event            
      d_ptr->m_lastMovePos = event->pos();
	
      // Filter out too smal movements        
      if ( ( d_ptr->m_orientation == Qt::Horizontal && fabs( deltaX ) == 0 ) ||
	   ( d_ptr->m_orientation == Qt::Vertical   && fabs( deltaY ) == 0 ) ){
	return;
      }
        
      // translate the view by delta
      translate( deltaX, deltaY );	
    }
  }

  void PannableView::scroll( qreal value ){
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
    // TODO: FIX THIS LOOK NICER. REMOVE HARDCODING
    if ( d_ptr->m_orientation == Qt::Horizontal ){
    if ( fabs(xform.m31()) < 2 ){    
      d_ptr->m_scrollingTimeLine->stop();
      return;
    }else
    if ( ( fabs(xform.m31()) + 800 ) >= scene()->sceneRect().width() ){	
      d_ptr->m_scrollingTimeLine->stop();
      return;
    }
    }

    // TODO: HANDLE VERTICAL SCROLLING ENDING HERE:

    // TODO: FIX THIS TO LOOK NICER
    // Scroll to the left or right
    if ( d_ptr->m_orientation == Qt::Horizontal ){
      if ( d_ptr->m_deltaX > 0 ){
	translate( value*100, 0 );
      }else
      if ( d_ptr->m_deltaX < 0 ){
	translate( value*-100, 0 );
      }
    }else{
      if ( d_ptr->m_deltaY > 0 ){
	translate( 0, value*100 );
      }else
      if ( d_ptr->m_deltaY < 0 ){
	translate( 0, value*-100 );
      }
    }

    
  }

}
