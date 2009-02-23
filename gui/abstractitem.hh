#ifndef ABSTRACTITEM_HH
#define ABSTRACTITEM_HH
#include "globals.hh"

#include <QGraphicsWidget>

namespace qtablet{


class AbstractItemPrivate;


//! AbstractItem class is a super class for all the items, which should react on orientation changes.
//!
//!
class AbstractItem : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotationZ READ rotationZ WRITE setRotationZ )

 public:
    AbstractItem( QGraphicsItem * parent = 0 );

    virtual ~AbstractItem();

    //virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const = 0;

    virtual qreal rotationZ() const;

    virtual void setRotationZ(qreal rr);

    virtual void addItemAttribute( QString const & key, QVariant const & value );

    virtual QVariant itemAttribute( QString const & key ) const;

private:
    AbstractItemPrivate * d_ptr;
    Q_DISABLE_COPY( AbstractItem );

};

}
#endif // ABSTRACTITEM_H
