#ifndef IMAGEITEM_HH
#define IMAGEITEM_HH

#include <QGraphicsWidget>
#include <QtDebug>
#include "abstractitem.hh"

namespace qtablet{

class ImageItemPrivate;

class ImageItem: public AbstractItem{

public:
    ImageItem( QString const & imagePath, QGraphicsItem * parent = 0 );

    virtual ~ImageItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

private:
    ImageItemPrivate * d_ptr;

};

}

#endif // IMAGEITEM_H
