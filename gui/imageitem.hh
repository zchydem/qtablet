#ifndef IMAGEITEM_HH
#define IMAGEITEM_HH

#include <QGraphicsWidget>
#include <QtDebug>
#include "abstractitem.hh"

namespace qtablet{

class ImageItemPrivate;

class ImageItem: public AbstractItem{

public:
    ImageItem( QGraphicsItem * parent = 0 );

    ImageItem( QString const & imagePath, QGraphicsItem * parent = 0 );

    ImageItem( QPixmap const & pixmap, QGraphicsItem * parent = 0 );
    
    virtual ~ImageItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QSizeF imageSize() const;

    bool isNull() const;

    //virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    virtual void setImage( QString const & path );

    virtual void setImage( QPixmap const & pixmap );

private:
    ImageItemPrivate * d_ptr;
    Q_DISABLE_COPY( ImageItem );
};

}

#endif // IMAGEITEM_H
