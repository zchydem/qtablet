#include "abstractitem.hh"
#include <QTimer>
#include <QTime>
#include <QtDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace qtablet{

class AbstractItemPrivate{
public:
    AbstractItemPrivate():
    m_currentRotation(0)
    {
    }

    qreal m_currentRotation;
};

// AbstractItem class
AbstractItem::AbstractItem( QGraphicsItem * parent ):
    QGraphicsWidget( parent ),
    d_ptr( new AbstractItemPrivate )
{

}

AbstractItem::~AbstractItem(){
    delete d_ptr;
    d_ptr = 0;
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
