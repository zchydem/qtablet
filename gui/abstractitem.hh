#ifndef ABSTRACTITEM_HH
#define ABSTRACTITEM_HH
#include "globals.hh"

#include <QGraphicsWidget>

namespace qtablet{

class AbstractItemPrivate;

//! AbstractItem class is a super class for all the items, which should react on orientation changes.
//!
//! This class provides default implementations for the mouse events and provides rotation methods
//! as pre-defined properties. Because rotation methods are properties subclasses of this class
//! can animate e.g. rotation automatically using Qt Animation Framework i.e. Qt Kinetic.
//!
//! This class also provides the default mechanism for activating the context menu i.e. when the
//! item is being pressed the default time out.
class AbstractItem : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotationZ READ rotationZ WRITE setRotationZ )
public:
    AbstractItem( QGraphicsItem * parent = 0 );

    virtual ~AbstractItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ) = 0;

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const = 0;

    virtual qreal rotationZ() const;

    virtual void setRotationZ(qreal rr);
protected:
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

signals:
    void mousePressedTimeout();
    void clicked();

private:
    AbstractItemPrivate * d_ptr;

};

}
#endif // ABSTRACTITEM_H
