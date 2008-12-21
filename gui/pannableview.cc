#include "pannableview2.hh"
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QTime>
#include <QPointF>
#include <QTimeLine>
#include <QtDebug>
#include <math.h>
#include <QPaintEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLayout>
#include <QRectF>



class PannableViewPrivate{
public:
    PannableViewPrivate():
        m_pannableWidget( 0 ){}

    qtablet::PannableWidget * m_pannableWidget;
};

class PannableWidgetPrivate{
public:
  PannableWidgetPrivate():
    m_acceleration( 0 ),
    m_deltaX( 0 ),
    m_deltaY( 0 ),
    m_time(),
    m_scrollingTimeLine( 0 ),
    m_mousePressPos(),
    m_isScrollingEnabled( false ),
    m_isPressed( false )
   {}

  // Members
  qreal           m_acceleration;
  qreal           m_deltaX;
  qreal           m_deltaY;
  QTime           m_time;
  QTimeLine     * m_scrollingTimeLine;
  QPointF         m_mousePressPos;
  bool            m_isScrollingEnabled;
  bool            m_isPressed;
  Qt::Orientation m_orientation;
};

namespace qtablet{

    PannableView::PannableView( Qt::Orientation orientation, qreal width, qreal height, QGraphicsItem * parent ):
    QGraphicsWidget( parent ),
    d_ptr( new PannableViewPrivate )
    {
        setMinimumSize( width, height );
        setMaximumSize( width, height );
        setFlags( QGraphicsItem::ItemClipsChildrenToShape );

        d_ptr->m_pannableWidget = new PannableWidget( orientation, this );
    }

    PannableView::~PannableView(){
        delete d_ptr;
        d_ptr = 0;
    }

    void PannableView::setLayout( QGraphicsLayout * layout ){
        d_ptr->m_pannableWidget->setLayout( layout );
    }



  PannableWidget::PannableWidget( Qt::Orientation orientation, QGraphicsItem * parent ):
    QGraphicsWidget( parent ),
    d_ptr( new PannableWidgetPrivate )
  {
    d_ptr->m_orientation = orientation;
    d_ptr->m_scrollingTimeLine = new QTimeLine( 1000, this );
    d_ptr->m_scrollingTimeLine->setCurveShape( QTimeLine::EaseInCurve );
    d_ptr->m_scrollingTimeLine->setDirection( QTimeLine::Backward );
        
    connect( d_ptr->m_scrollingTimeLine, SIGNAL( valueChanged( qreal ) ),
	     this,                       SLOT  ( scroll( qreal ) ) );
  }
  
  PannableWidget::~PannableWidget(){
    delete d_ptr;
    d_ptr = 0;
  }


  QPainterPath PannableWidget::shape () const{
    QPainterPath path;
    QSizeF size = QGraphicsWidget::size();
    path.addRect( QRectF(0,0,size.width(), size.height() ) );
    return path;
  }
  


  void PannableWidget::mousePressEvent( QGraphicsSceneMouseEvent * event ){
    qDebug() << "PRESS";
    //QGraphicsItem::mousePressEvent( event );

    d_ptr->m_isPressed   = true;
    d_ptr->m_scrollingTimeLine->stop();
    d_ptr->m_mousePressPos  = event->scenePos();
    d_ptr->m_time.start();        
  }

  void PannableWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent * event ){
    qDebug() << "RELEASE";

    d_ptr->m_isPressed = false;

    qreal  dxD             = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).x(); // Delta X distance
    qreal  dyD             = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).y(); // Delta Y distance
    qint32 tD              =  d_ptr->m_time.elapsed() ;                   // Delta Time

    //qDebug() << event->scenePos() << event->buttonDownScenePos(Qt::LeftButton) << d_ptr->m_mousePressPos;

    // Calculate acceleration
    if ( d_ptr->m_orientation == Qt::Horizontal ){
      d_ptr->m_acceleration = fabs( dxD / tD ); 
    }else{
      d_ptr->m_acceleration = fabs( dyD / tD );
    }
        
    qint32 multiplier = 1000;
    //qDebug() << "dD: " << dxD << "tD: " << tD << "a:" << d_ptr->m_acceleration ;
    if ( fabs(dxD) > 20 || fabs(dyD) > 20 ){

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
    //QGraphicsItem::mouseReleaseEvent( event );

  }

  void PannableWidget::mouseMoveEvent(  QGraphicsSceneMouseEvent * event ){

    if ( d_ptr->m_isPressed ){

      qreal deltaX = 0;
      qreal deltaY = 0;

      if ( layout() == 0 ){
	return;
      }

      qDebug() << geometry() << layout()->geometry();


      if ( d_ptr->m_orientation == Qt::Horizontal ){
          deltaX = ( event->scenePos()  - event->lastScenePos() ).x();
      }else{
          deltaY = ( event->scenePos()  - event->lastScenePos()).y();
      }
	
      // Filter out too small movements
      if ( ( d_ptr->m_orientation == Qt::Horizontal && fabs( deltaX ) == 0 ) ||
	   ( d_ptr->m_orientation == Qt::Vertical   && fabs( deltaY ) == 0 ) ){
	return;
      }

      // translate the view by delta
      QRectF geom = geometry();
      geom.translate( deltaX, deltaY );
      setGeometry( geom );

    }
  }



  void PannableWidget::scroll( qreal value ){
    // Calculate the start and end positions
    // based on the current transformation matrix
    // and the width of the scene
    //QTransform xform = viewportTransform();

    if ( value == 0 ){
      d_ptr->m_acceleration = 0;
      return;
    }

    qDebug() << geometry() << layout()->geometry();

    if ( layout() == 0 ){
      return;
    }

    if ( d_ptr->m_orientation == Qt::Horizontal ){

        qreal viewX       = geometry().x();
        qreal layoutWidth = layout()->geometry().width();
        qDebug() << viewX << layoutWidth;

        if ( d_ptr->m_deltaX > 0 && viewX <= layoutWidth){
            // Stop scrolling to right
            //d_ptr->m_scrollingTimeLine->stop();
        }

        if ( d_ptr->m_deltaX < 0 && viewX <= 0 ){
            // Stop scrolling to left
            //d_ptr->m_scrollingTimeLine->stop();
        }

    }else
    if ( d_ptr->m_orientation == Qt::Vertical ){
        qreal viewY        = geometry().y();
        qreal layoutHeight = layout()->geometry().height();

        if ( d_ptr->m_deltaY > 0 && viewY <= layoutHeight){
            // Stop scrolling to up
            d_ptr->m_scrollingTimeLine->stop();
        }

        if ( d_ptr->m_deltaY < 0 && viewY <= 0 ){
            // Stop scrolling to down
            d_ptr->m_scrollingTimeLine->stop();
        }
    }

    QRectF geom = geometry();

    // TODO: FIX THIS TO LOOK NICER
    // Scroll to the left or right
    if ( d_ptr->m_orientation == Qt::Horizontal ){
      if ( d_ptr->m_deltaX > 0 ){
	geom.translate( value*100, 0 );
      }else
      if ( d_ptr->m_deltaX < 0 ){
	geom.translate( value*-100, 0 );
      }
    }else{
      if ( d_ptr->m_deltaY > 0 ){
	geom.translate( 0, value*100 );
	
      }else
      if ( d_ptr->m_deltaY < 0 ){
	geom.translate( 0, value*-100 );
      }
    }

    setGeometry( geom );
    
    }
}
