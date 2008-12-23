#ifndef IMAGEITEM_HH
#define IMAGEITEM_HH

#include <QGraphicsWidget>
#include <QtDebug>

namespace qtablet{

class ImageItemPrivate;

class ImageItem: public QGraphicsWidget{

public:
    ImageItem( QString const & imagePath, QGraphicsItem * parent = 0 );

    virtual ~ImageItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:
    ImageItemPrivate * d_ptr;

};

}
#endif // IMAGEITEM_H
