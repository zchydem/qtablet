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

#define PANNABLE_FRICTION_COEFFICIENT 0.15 * 9.81  // F = y*N, 0.1 = friction coefficient for ice on ice.

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
    m_vX               (   0   ),
    m_vY               (   0   ),
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
    qreal              m_vX;
    qreal              m_vY;
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
    d_ptr->m_scrollingTimeLine->setCurveShape( QTimeLine::LinearCurve );

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

    // If scrolling is stopped but not in this method call, make sure that item if there
    // is any, get's the mouse press event. Otherwise make the previously selected
    // item unselected.
    PannableViewItem * item = qgraphicsitem_cast<PannableViewItem*>(scene()->itemAt( event->scenePos() ));
    bool isThisWidget       = qgraphicsitem_cast<QGraphicsWidget*>( item ) == qgraphicsitem_cast<QGraphicsWidget*>( this );

    if ( !stopped && !isThisWidget && item != 0 && item->acceptMouseEvent() ){
        d_ptr->m_selectedItem = item;
        d_ptr->m_selectedItem->pannableViewMousePressEvent( event );
    }else{
        d_ptr->m_selectedItem = 0;
    }

    d_ptr->m_isPressed  = true;
    d_ptr->m_scrollingTimeLine->setCurrentTime(0);
    d_ptr->m_time.start();
}

void PannableWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent * event ){

    d_ptr->m_isPressed = false;

    PannableViewItem * item = qgraphicsitem_cast<PannableViewItem*>( scene()->itemAt( event->scenePos() ) );
    if ( d_ptr->m_selectedItem != 0 && d_ptr->m_selectedItem == item && item != 0 && item->acceptMouseEvent() ){
        d_ptr->m_selectedItem->pannableViewMouseReleaseEvent( event );
        d_ptr->m_selectedItem = 0;
    }

    qint32 tD  =  d_ptr->m_time.elapsed();  // Delta Time
    qreal  dxD = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).x(); // Delta X distance
    qreal  dyD = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton) ).y(); // Delta Y distance
    if ( tD == 0 ){
        return;
    }

    /*
    if(tD > 200 && (dxD < 100 || dyD < 100)) {  // If the touch is >0.2s then it is probably not a flick -> do not pan.
                                                // and the flick is short.
                                                // TODO: Check if 0.2s is ok.

        return;

    }
    */
    qreal vx = (dxD / tD ) * 1000;          // Velocity in x dimension (pixels / second).
    qreal vy = (dyD / tD ) * 1000;          // Velocity in y dimension (pixels / second).

    if ( fabs( vx ) > 1.0 || fabs(vy) > 1.0 ){
        qreal velocity = 0;  // Current velocity in x or y dimension.

        d_ptr->m_deltaX = dxD;
        d_ptr->m_deltaY = dyD;

        d_ptr->m_vX = vx;
        d_ptr->m_vY = vy;

        if ( d_ptr->m_orientation & Qt::Horizontal ){
            velocity = fabs( vx );
        }else{
            velocity = fabs( vy );
        }

        if ( velocity > 0 ){
            d_ptr->m_scrollingTimeLine->setDuration(  static_cast<qint32>( ( velocity/(PANNABLE_FRICTION_COEFFICIENT) ) ) );
            d_ptr->m_scrollingTimeLine->start();
        }
        return;
    }
}

void PannableWidget::mouseMoveEvent(  QGraphicsSceneMouseEvent * event ){    

    if ( !d_ptr->m_isPressed ){
        return;
    }

    // If moving mouse while some item is selected, we don't want to pan the view.
    // Just pass the event to the item and exit.
    if ( d_ptr->m_selectedItem != 0 ){        
        d_ptr->m_selectedItem->pannableViewMouseMoveEvent( event );        
        //return;
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

    if ( d_ptr->m_orientation & Qt::Horizontal ){
        // s = 1/2 * (u + v)t,
        // u=0,
        // t=update interval (in ms) of the QTimeLine.
        qreal s = d_ptr->m_vX * (d_ptr->m_scrollingTimeLine->updateInterval()/ 2) / 1000;

        // v = u + at,
        // u = d_ptr->m_vX (current speed),
        // a = yN/m, m = 1kg (just to have something), yN = friction
        qreal v = ( fabs(d_ptr->m_vX) - (PANNABLE_FRICTION_COEFFICIENT * d_ptr->m_scrollingTimeLine->updateInterval()) );
        d_ptr->m_vX = (d_ptr->m_vX < 0 ? -v : v);

        moveBy(s, 0);
    } else {
        // s = 1/2 * (u + v)t,
        // u=0,
        // t=update interval (in ms) of the QTimeLine.
        qreal s = d_ptr->m_vY * (d_ptr->m_scrollingTimeLine->updateInterval()/ 2) / 1000;

        // v = u + at,
        // u = d_ptr->m_vY (current speed),
        // a = yN/m, m = 1kg (just to have something), yN = friction
        qreal v = ( fabs(d_ptr->m_vY) - (PANNABLE_FRICTION_COEFFICIENT * d_ptr->m_scrollingTimeLine->updateInterval()) );
        d_ptr->m_vY = (d_ptr->m_vY < 0 ? -v : v);

        moveBy(0, s);
    }

    // Let's calculate next, if  we need to stop scrolling and if so,
    // calculate the end position also based on the parent view size.
    QRectF geom = geometry();
    qreal viewWidth    = parentItem()->boundingRect().width();
    qreal viewHeight   = parentItem()->boundingRect().height();
    qreal layoutWidth  = layout()->geometry().width();
    qreal layoutHeight = layout()->geometry().height();

    // TODO: If panning is kept like the implementation above, this must be checked.
    if ( d_ptr->m_orientation & Qt::Horizontal ){

        if ( d_ptr->m_deltaX < 0 && ( ( fabs( geom.x() ) + viewWidth ) > layoutWidth )){
            // Moving right
            geom.setX( viewWidth - layoutWidth );
            d_ptr->m_scrollingTimeLine->stop();
            setGeometry( geom );
        }

        if ( d_ptr->m_deltaX > 0 && geom.x() > 0 ){
            // Moving left
            geom.setX( 0 );
            d_ptr->m_scrollingTimeLine->stop();            
            setGeometry( geom );
        }
    }

    if ( d_ptr->m_orientation & Qt::Vertical ){

        if ( d_ptr->m_deltaY < 0 && ( ( fabs(geom.y()) + viewHeight ) > layoutHeight )){
            // Moving down
            geom.setY( viewHeight - layoutHeight );
            d_ptr->m_scrollingTimeLine->stop();            
            setGeometry( geom );
        }

        if (  d_ptr->m_deltaY > 0 && geom.y() > 0 ){
            // Moving up
            geom.setY( 0 );
            d_ptr->m_scrollingTimeLine->stop();
            setGeometry( geom );
        }
    }
}

void PannableWidget::animateEnd(){
    //TODO: Implement this
}

}

