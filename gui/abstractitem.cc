#include "abstractitem.hh"
#include <QTimer>
#include <QtDebug>

namespace qtablet{

class AbstractItemPrivate{
public:
    AbstractItemPrivate():
    m_currentRotation(0),
    m_pressTimer( new QTimer ){}

    ~AbstractItemPrivate(){
        delete m_pressTimer;
    }
    qreal   m_currentRotation;
    QTimer *m_pressTimer;
};

AbstractItem::AbstractItem( QGraphicsItem * parent ):
    QGraphicsWidget( parent ),
    d_ptr( new AbstractItemPrivate )
{
}

AbstractItem::~AbstractItem(){
    delete d_ptr;
    d_ptr = 0;
}

/*
void AbstractItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ){

}

QSizeF AbstractItem::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{

}
*/
void AbstractItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ){
    QGraphicsItem::mousePressEvent( event );
    qDebug() << "AbstractItem::pressed";
}

void AbstractItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ){
    //QGraphicsItem::mouseReleaseEvent( event );
    qDebug() << "AbstractItem::Released";
    emit clicked();
}

qreal AbstractItem::rotationZ() const{
        return d_ptr->m_currentRotation;
}

void AbstractItem::setRotationZ(qreal rr){
        d_ptr->m_currentRotation = rr;
        QPointF c = boundingRect().center();
        setTransform(QTransform().translate(c.x(), c.y()).rotate(rr, Qt::ZAxis).translate(-c.x(), -c.y()));
}

}
