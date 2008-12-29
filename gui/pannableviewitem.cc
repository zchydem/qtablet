#include "pannableviewitem.hh"

#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>

namespace qtablet{

class PannableViewItemPrivate{
public:
    PannableViewItemPrivate():m_timer(0), m_timerId(0){
        m_timer = new QTimer;
    }

    ~PannableViewItemPrivate(){
        delete m_timer;
        m_timer = 0;
    }

    QTimer  * m_timer; 
    qint32    m_timerId;
};


PannableViewItem::PannableViewItem( QGraphicsItem * parent ):
    AbstractItem( parent ),
    d_ptr( new PannableViewItemPrivate )
{

}


PannableViewItem::~PannableViewItem(){
    delete d_ptr;
}

void PannableViewItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ){
    //qDebug() << "PannableViewItem::paint";
    /*
    if ( 0 != d_ptr->m_timerId ){
        painter->setPen( QPen( Qt::red, 4 ) );
        painter->drawRect( boundingRect() );
    }
    */
}


void PannableViewItem::pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event ){
    Q_UNUSED( event );
    //d_ptr->m_timerId = this->startTimer( 1000 );
    qDebug() << "Item pressed";
}

void PannableViewItem::pannableViewMouseReleaseEvent( QGraphicsSceneMouseEvent * event ){
    Q_UNUSED( event );
    /*
    killTimer( d_ptr->m_timerId );
    d_ptr->m_timerId = 0;
    emit clicked();
    update();
    */
    emit clicked();
    qDebug() << "Item Released";
}

void PannableViewItem::pannableViewMousePressCancelled(){
    // TODO: Ignore press or release events if this methods is called.
    /*
    killTimer( d_ptr->m_timerId );
    d_ptr->m_timerId = 0;
    update();
    */
    qDebug() << "Item mouse events cancelled";
}

void PannableViewItem::timerEvent( QTimerEvent * event ){
    Q_UNUSED( event );
    //update();
}

}
