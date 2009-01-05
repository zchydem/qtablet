#include "imageitem.hh"
#include <QPainter>

namespace qtablet{

class ImageItemPrivate{
public:
    ImageItemPrivate():m_pixmap(){}
    ImageItemPrivate( QString const & filePath ):m_pixmap( filePath ){}
    QPixmap m_pixmap;
};

ImageItem::ImageItem( QGraphicsItem * parent):
    AbstractItem( parent ),
    d_ptr(new ImageItemPrivate )
{
}

ImageItem::ImageItem( QString const & imagePath, QGraphicsItem * parent ):
        AbstractItem( parent ),
        d_ptr( new ImageItemPrivate( imagePath ) )
{
    if ( d_ptr->m_pixmap.isNull() ){
        qCritical() << "ImageItem::ImageItem: NULL QPixmap";
    }
}

ImageItem::~ImageItem(){
    delete d_ptr;
}

void ImageItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED( option );
    Q_UNUSED( widget );    
    painter->drawPixmap( 0, 0, d_ptr->m_pixmap );
}

QSizeF ImageItem::imageSize() const{
    return d_ptr->m_pixmap.size();
}

bool ImageItem::isNull() const{
    return d_ptr->m_pixmap.isNull();
}


QSizeF ImageItem::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return QSizeF( d_ptr->m_pixmap.width(), d_ptr->m_pixmap.height() );
}

void ImageItem::setImage( QString const & path ){
    if ( !d_ptr->m_pixmap.load( path ) ){
        qCritical() << "ImageItem::setImage: failed to load pixmap: " << path;
        return;
    }

    update();
}
}
