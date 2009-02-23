#include "labelitem.hh"

#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QPixmapCache>
#include <QPainter>
#include <QTextOption>
#include <QtDebug>

namespace qtablet{
class LabelItemPrivate{
    public:
    LabelItemPrivate():
    m_text     ( ""            ),
    m_font     (               ),
    m_color    ( Qt::black     ),
    m_alignment( Qt::AlignLeft ),
    m_width    (       0       )
    {}

    QString key(){
        QString result;
        result.sprintf("%p", static_cast<const void *>(this));
        return result;
    }

    QPixmap generatePixmap(){

        QTextOption option( m_alignment );
        quint32 height;

        option.setWrapMode( QTextOption::WrapAnywhere );

        if ( textWidth() > m_width && m_width != 0 ){
            //option.setWrapMode( QTextOption::WrapAnywhere );
            height = textHeight() * ( textWidth() / ((float)(m_width)) );
        }else{
            height = textHeight();
        }


        QPixmap pixmap( textWidth(), textHeight() );
        QColor color(0,0,0,0);
        pixmap.fill( color );
        QPainter painter( &pixmap );

        painter.setFont ( m_font  );
        painter.setPen( m_color );
        painter.drawText( pixmap.rect(), m_text, option );
        return pixmap;
    }

    quint32 textWidth(){
        QFontMetrics metrics( m_font );
        return metrics.width( m_text );
    }

    quint32 textHeight(){
        if ( m_text.isEmpty() ){
            return 0;
        }
        QFontMetrics metrics( m_font );
        return metrics.height();
    }

    // Members
    QString       m_text;
    QFont         m_font;
    QColor        m_color;
    Qt::Alignment m_alignment;
    quint32       m_width;
};

LabelItem::LabelItem( QGraphicsItem * parent ):
AbstractItem( parent ),
d_ptr( new LabelItemPrivate )
{
    d_ptr->m_text  = "";
    d_ptr->m_font  = font();
}


LabelItem::LabelItem( QString const & text, QGraphicsItem * parent ):
AbstractItem( parent ),
d_ptr( new LabelItemPrivate )
{
    d_ptr->m_text  = text;
    d_ptr->m_font  = font();
}


LabelItem::~LabelItem(){

}

void LabelItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED( option );
    Q_UNUSED( widget );

    QPixmap pixmap;
    if ( !QPixmapCache::find( d_ptr->key(), pixmap ) ){        
        pixmap = d_ptr->generatePixmap();
        QPixmapCache::insert( d_ptr->key(), pixmap );                
    }

    painter->drawPixmap( 0, 0, pixmap );
}

void LabelItem::setText( QString const & text ){
    d_ptr->m_text = text;
    updateLabel();
}

void LabelItem::setFont( QFont const & font ){
    d_ptr->m_font = font;
    updateLabel();
}

void LabelItem::setColor( QColor const & color ){
    d_ptr->m_color = color;
    updateLabel();
}

void LabelItem::setAlignment( Qt::Alignment const & alignment ){
    d_ptr->m_alignment = alignment;
    updateLabel();
}

void LabelItem::setWidth( quint32 width ){
    d_ptr->m_width = width;
    updateLabel();
}

void LabelItem::clear(){
    setText("");
    updateLabel();
}

QString const & LabelItem::text() const{
    return d_ptr->m_text;
}

QFont  const & LabelItem::font() const{
    return d_ptr->m_font;
}

QColor const & LabelItem::color() const{
    return d_ptr->m_color;
}

Qt::Alignment const & LabelItem::alignment() const{
    return d_ptr->m_alignment;
}

void LabelItem::updateLabel(){
    QPixmapCache::remove( d_ptr->key() );
    QPixmap pixmap = d_ptr->generatePixmap();
    QPixmapCache::insert( d_ptr->key(), pixmap );
    setMinimumSize  ( pixmap.size() );
    setMaximumSize  ( pixmap.size() );
    setPreferredSize( pixmap.size() );
    updateGeometry();
    update();
}

bool LabelItem::isEmpty() const{
    return d_ptr->m_text.isEmpty();
}
}
