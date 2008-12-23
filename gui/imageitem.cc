#include "imageitem.hh"
#include <QPainter>

namespace qtablet{

class ImageItemPrivate{
public:
    ImageItemPrivate( QString const & filePath ):m_pixmap( filePath ){};
    QPixmap m_pixmap;
};

ImageItem::ImageItem( QString const & imagePath, QGraphicsItem * parent ):
        QGraphicsWidget(parent),
        d_ptr( new ImageItemPrivate( imagePath ) )
{
    if ( d_ptr->m_pixmap.isNull() ){
        qCritical() << "NULL QPixmap";
    }
}

ImageItem::~ImageItem(){

}

void ImageItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED( option );
    Q_UNUSED( widget );

    painter->drawPixmap( 0, 0, d_ptr->m_pixmap );
}

QSizeF ImageItem::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return QSizeF( d_ptr->m_pixmap.width(), d_ptr->m_pixmap.height() );
}

void ImageItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ){
    QGraphicsWidget::mousePressEvent( event );
}

void ImageItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ){
    QGraphicsWidget::mouseReleaseEvent( event );
}

}
