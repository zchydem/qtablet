#include "pannableview.hh"
#include "pannableviewitem.hh"
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
#include <QCoreApplication>

namespace qtablet{

class PannableViewPrivate{
public:
PannableViewPrivate():
    m_pannableWidget( 0 ){}

qtablet::PannableWidget * m_pannableWidget;
};

class PannableWidgetPrivate{
public:
    PannableWidgetPrivate():
    m_acceleration     (   0   ),
    m_deltaX           (   0   ),
    m_deltaY           (   0   ),
    m_time             (       ),
    m_scrollingTimeLine(   0   ),
    m_isPressed        ( false ),
    m_orientation      (   0   ),
    m_selectedItem     (   0   )
    {}

    // Members
    qreal              m_acceleration;
    qreal              m_deltaX;
    qreal              m_deltaY;
    QTime              m_time;
    QTimeLine        * m_scrollingTimeLine;    
    bool               m_isPressed;
    Qt::Orientations   m_orientation;
    PannableViewItem * m_selectedItem;
};


//// Pannable View Implementation ////
PannableView::PannableView( Qt::Orientations orientation, qreal width, qreal height, QGraphicsItem * parent ):
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


//// Pannable Widget Implementation ////
PannableWidget::PannableWidget( Qt::Orientations orientation, QGraphicsItem * parent ):
    QGraphicsWidget( parent ),
    d_ptr( new PannableWidgetPrivate )
{
    setFlags( QGraphicsItem::ItemClipsChildrenToShape );
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

    // Press event stops the scrolling
    bool stopped = false;
    if ( d_ptr->m_scrollingTimeLine->state() == QTimeLine::Running ){
        d_ptr->m_scrollingTimeLine->stop();
        stopped = true;
    }

    // Make item aware of press event if there is an item and the scrolling hasn't been stopped
    // this int event.
    QGraphicsWidget * widget = static_cast<QGraphicsWidget*>(scene()->itemAt( event->scenePos() ));
    if ( !stopped && widget != this && widget != 0){
        PannableViewItem * item = static_cast<PannableViewItem*>( scene()->itemAt( event->scenePos() ) );
        d_ptr->m_selectedItem = item;
        d_ptr->m_selectedItem->pannableViewMousePressEvent( event );
    }else{
        d_ptr->m_selectedItem = 0;
    }

    d_ptr->m_isPressed  = true;
    d_ptr->m_time.start();
}

void PannableWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent * event ){

    d_ptr->m_isPressed = false;

    PannableViewItem * item = static_cast<PannableViewItem*>( scene()->itemAt( event->scenePos() ) );
    if ( d_ptr->m_selectedItem != 0 && d_ptr->m_selectedItem == item && item != 0 ){
        d_ptr->m_selectedItem->pannableViewMouseReleaseEvent( event );
        d_ptr->m_selectedItem = 0;                
    }

    qreal  dxD = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).x(); // Delta X distance
    qreal  dyD = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).y(); // Delta Y distance
    qint32 tD  =  d_ptr->m_time.elapsed() ;                                            // Delta Time


    // Calculate acceleration
    if ( d_ptr->m_orientation & Qt::Horizontal ){
      d_ptr->m_acceleration = fabs( dxD / tD );
    }else{
      d_ptr->m_acceleration = fabs( dyD / tD );
    }

    // Start to scroll only if delta values are more that 20 px
    if ( fabs(dxD) > 20 || fabs(dyD) > 20 ){

        d_ptr->m_deltaX = dxD;
        d_ptr->m_deltaY = dyD;
        qint32 multiplier = 1000;

        // Magic multiplier value is based on the acceleration. These are just values
        // which have been found by trying things out.
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
        if ( multiplier != 0 ){
            d_ptr->m_scrollingTimeLine->setDuration(  static_cast<qint32>( d_ptr->m_acceleration * multiplier ) );
            d_ptr->m_scrollingTimeLine->start();
        }        
    }
}

void PannableWidget::mouseMoveEvent(  QGraphicsSceneMouseEvent * event ){    

    if ( !d_ptr->m_isPressed ){
        return;
    }

    qreal deltaX = 0;
    qreal deltaY = 0;

    if ( layout() == 0 ){
        return;
    }

    QPointF delta = event->scenePos()  - event->lastScenePos();

    if ( d_ptr->m_orientation & Qt::Horizontal ){
        deltaX = delta.x();
    }

    if ( d_ptr->m_orientation & Qt::Vertical ){
        deltaY = delta.y();
    }

    // Filter out too small movements
    if ( ( d_ptr->m_orientation & Qt::Horizontal && fabs( deltaX ) == 0 ) ||
       ( d_ptr->m_orientation & Qt::Vertical   && fabs( deltaY ) == 0 ) ){
        return;
    }

    // We are moving, not selecting the item. Reset the currently selected item if there's a one.    
    if ( d_ptr->m_selectedItem != 0 ){
        d_ptr->m_selectedItem->pannableViewMousePressCancelled();
        d_ptr->m_selectedItem = 0;
    }

    // translate the view by delta
    QRectF geom = geometry();
    geom.translate( deltaX, deltaY );
    setGeometry( geom );
}


void PannableWidget::scroll( qreal value ){

    if ( value == 0 ){
        d_ptr->m_acceleration = 0;
        return;
    }

    if ( layout() == 0 ){
        return;
    }

    QRectF geom = geometry();

    // Scroll to the left or right by translating geometry
    // x and y positions. We use 100 here as a multiplier.
    if ( d_ptr->m_orientation & Qt::Horizontal ){
        if ( d_ptr->m_deltaX > 0 ){
            geom.translate( value*100, 0 );
        }else
        if ( d_ptr->m_deltaX < 0 ){
            geom.translate( value*-100, 0 );
        }
    }

    if ( d_ptr->m_orientation & Qt::Vertical ){
        if ( d_ptr->m_deltaY > 0 ){
          geom.translate( 0, value*100 );
        }else
        if ( d_ptr->m_deltaY < 0 ){
          geom.translate( 0, value*-100 );
        }
    }

    // Let's calculate next, if  we need to stop scrolling and if so,
    // calculate the end position also based on the parent view size.
    qreal viewWidth    = parentItem()->boundingRect().width();
    qreal viewHeight   = parentItem()->boundingRect().height();
    qreal layoutWidth  = layout()->geometry().width();
    qreal layoutHeight = layout()->geometry().height();

    if ( d_ptr->m_orientation & Qt::Horizontal ){

        if ( d_ptr->m_deltaX < 0 && ( ( fabs( geom.x() ) + viewWidth ) > layoutWidth )){
            // Moving right
            geom.setX( viewWidth - layoutWidth );
            d_ptr->m_scrollingTimeLine->stop();
        }

        if ( d_ptr->m_deltaX > 0 && geom.x() > 0 ){
            // Moving left
            geom.setX( 0 );
            d_ptr->m_scrollingTimeLine->stop();            
        }
    }

    if ( d_ptr->m_orientation & Qt::Vertical ){

        if ( d_ptr->m_deltaY < 0 && ( ( fabs(geom.y()) + viewHeight ) > layoutHeight )){
            // Moving down
            geom.setY( viewHeight - layoutHeight );
            d_ptr->m_scrollingTimeLine->stop();            
        }

        if (  d_ptr->m_deltaY > 0 && geom.y() > 0 ){
            // Moving up
            geom.setY( 0 );
            d_ptr->m_scrollingTimeLine->stop();
        }
    }

    // And finally set new x or y to this widget.
    setGeometry( geom );
}

void PannableWidget::animateEnd(){
    //TODO: Implement this
}

}

