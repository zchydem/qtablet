#include "toolbarbutton.hh"

#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QPainter>
#include <QColor>
#include <QFont>


namespace qtablet{

class ToolbarButtonPrivate{
public:
    ToolbarButtonPrivate(ToolbarButton::ButtonType type ):
    m_isPressed( false ),
    m_pressedColor( Qt::darkGray ),
    m_releasedColor( Qt::lightGray ),
    m_font("Arial", 28, 55 ),
    m_rect(0,0,90,90)
    {
        m_pressedColor.setAlphaF( 0.5f );
        m_releasedColor.setAlphaF(0.3f );

        switch( type ){
        case ToolbarButton::QLauncherButton:
            m_char = "Q";
            break;
        case ToolbarButton::QPagerButton:
            m_char = "P";
            break;
        case ToolbarButton::MinimizeButton:
            m_char = "-";
            break;
        case ToolbarButton::CloseButton:
            m_char = "X";
            break;
        case ToolbarButton::MenuButton:
            m_char = "_";
            break;
        default:
            m_char = "?";
            break;
        }
    }

    bool    m_isPressed;
    QColor  m_pressedColor;
    QColor  m_releasedColor;
    QFont   m_font;
    QRectF  m_rect;
    QString m_char;
};


ToolbarButton::ToolbarButton( ButtonType type, QGraphicsItem * parent ):
AbstractItem( parent ),
d_ptr( new ToolbarButtonPrivate( type ) )
{

}

ToolbarButton::~ToolbarButton(){
    delete d_ptr;
    d_ptr = 0;
}

void ToolbarButton::paint(QPainter *painter, const QStyleOptionGraphicsItem * const option, QWidget * widget ){
    Q_UNUSED( option );
    Q_UNUSED( widget );

    // Let's draw the button
    if ( d_ptr->m_isPressed ){
        painter->setBrush( d_ptr->m_pressedColor );
    }else{
        painter->setBrush( d_ptr->m_releasedColor );
    }

    painter->setFont( d_ptr->m_font );
    painter->setPen(Qt::black );
    painter->drawRoundedRect( d_ptr->m_rect, 15, 15 );
    painter->drawText( d_ptr->m_rect, d_ptr->m_char, QTextOption(Qt::AlignCenter | Qt::AlignHCenter ) );

}

void ToolbarButton::resize( QSizeF const & size ){
    d_ptr->m_rect.setSize( size );   
    updateGeometry();
    update();
}

QSizeF ToolbarButton::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return d_ptr->m_rect.size();
}

void ToolbarButton::mousePressEvent( QGraphicsSceneMouseEvent *event ){
    Q_UNUSED( event );
    // Make this item a mouse grabber by defining this method in
    // order to receive mouseReleaseEvent.
    d_ptr->m_isPressed = true;
    update();
}

void ToolbarButton::mouseReleaseEvent( QGraphicsSceneMouseEvent *event ){
    Q_UNUSED( event );
    if ( d_ptr->m_isPressed ){
        d_ptr->m_isPressed = false;
        update();
        emit clicked();
    }
}

void ToolbarButton::mouseMoveEvent( QGraphicsSceneMouseEvent *event ){

    if ( !d_ptr->m_rect.contains( event->pos() ) ){
        d_ptr->m_isPressed = false;
        update();
    }
}

}
