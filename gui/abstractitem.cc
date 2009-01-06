#include "abstractitem.hh"
#include <QTimer>
#include <QTime>
#include <QtDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QHash>

namespace qtablet{

class AbstractItemPrivate{
public:
    AbstractItemPrivate():
    m_currentRotation(0)
    {
    }

    qreal m_currentRotation;
    QHash <QString, QVariant> m_attributes;
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


void AbstractItem::addItemAttribute( QString const & key, QVariant const & value ){
    d_ptr->m_attributes.insert( key, value );
}

QVariant AbstractItem::itemAttribute( QString const & key ) const{
    return d_ptr->m_attributes.value( key );
}

}
