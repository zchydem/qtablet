#include "pannableviewitem.hh"
#include "imageitem.hh"
#include "labelitem.hh"

#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsLinearLayout>
#include <QPainterPath>

namespace qtablet{

class PannableViewItemPrivate{
public:
    PannableViewItemPrivate():
            m_isPressed( false ),
            m_cancelPressed( false ),
            m_mouseReleasePress( false ),
            m_showSelection( true ),
            m_acceptMouseEvent( true ),
            m_image( new ImageItem ),
            m_label( new LabelItem )
    {
        // Make sure that child items don't get events i.e. they are behind this item
        m_image->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
        m_label->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    }

    ~PannableViewItemPrivate(){
        if ( m_image ){
            delete m_image;
        }

        if ( m_label ){
            delete m_label;
        }
    }

    bool                      m_isPressed;
    bool                      m_cancelPressed;
    bool                      m_mouseReleasePress;
    bool                      m_showSelection;
    bool                      m_acceptMouseEvent;
    ImageItem               * m_image;
    LabelItem               * m_label;    
};


PannableViewItem::PannableViewItem(  QString const & imageFile, QString const & labelText, QGraphicsItem * parent ):
    AbstractItem( parent ),
    d_ptr( new PannableViewItemPrivate )
{

    // Set image for the image item
    d_ptr->m_image->setImage( imageFile );
    d_ptr->m_label->setText ( labelText );


    // Create layout and add items there
    QGraphicsLinearLayout * layout = new QGraphicsLinearLayout( Qt::Vertical );
    layout->setContentsMargins(0,0,0,0);

    if ( !d_ptr->m_image->isNull() ){
        layout->addItem( d_ptr->m_image );
        layout->setAlignment( d_ptr->m_image, Qt::AlignCenter );
    }

    if ( !d_ptr->m_label->isEmpty() ){
        // Set text and other settings for the text item
        QSizeF imageSize = d_ptr->m_image->maximumSize();
        d_ptr->m_label->setWidth( static_cast<qint32>( imageSize.width() ) );
        d_ptr->m_label->setAlignment( Qt::AlignCenter );
        d_ptr->m_label->setFont( QFont( "Arial", 18, 68 ) );
        layout->addItem( d_ptr->m_label );
        layout->setAlignment( d_ptr->m_label, Qt::AlignCenter );
    }

    // And finally set the layout.
    setLayout( layout );
}

PannableViewItem::PannableViewItem( QPixmap const & pixmap, QString const & labelText, QGraphicsItem * parent ):
    AbstractItem( parent ),
    d_ptr( new PannableViewItemPrivate )
{

    // Set image for the image item
    d_ptr->m_image->setImage( pixmap    );
    d_ptr->m_label->setText ( labelText );


    // Create layout and add items there
    QGraphicsLinearLayout * layout = new QGraphicsLinearLayout( Qt::Vertical );
    layout->setContentsMargins(0,0,0,0);


    if ( !d_ptr->m_image->isNull() ){
        layout->addItem( d_ptr->m_image );
        layout->setAlignment( d_ptr->m_image, Qt::AlignCenter );
    }

    if ( !d_ptr->m_label->isEmpty() ){
        // Set text and other settings for the text item
        QSizeF imageSize = d_ptr->m_image->maximumSize();
        //d_ptr->m_label->setWidth( static_cast<qint32>( imageSize.width() ) );
        d_ptr->m_label->setAlignment( Qt::AlignCenter );
        d_ptr->m_label->setFont( QFont( "Arial", 18, 68 ) );
        layout->addItem( d_ptr->m_label );
        layout->setAlignment( d_ptr->m_label, Qt::AlignCenter );
    }

    // And finally set the layout.
    setLayout( layout );
    layout->activate();


}

PannableViewItem::~PannableViewItem(){
    delete d_ptr;
}

void PannableViewItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ){
    Q_UNUSED( option );
    Q_UNUSED( widget );

    if ( !d_ptr->m_showSelection ){
        return;
    }

    if (  d_ptr->m_mouseReleasePress || ( !d_ptr->m_cancelPressed && d_ptr->m_isPressed )  ){
        QRectF bBox = boundingRect();
        bBox.moveLeft( bBox.left() - 10 );
        bBox.moveRight( bBox.right() + 10 );
        bBox.moveTop( bBox.top() - 10 );
        bBox.moveBottom( bBox.bottom() + 10 );
        painter->setPen( QPen( Qt::gray, 4 ) );
        painter->drawRoundRect(bBox, 10, 10 );;
    }

    // Let's set flag to false here in order to enable fast tappings i.e. user
    // sees the selection of the item
    if ( d_ptr->m_isPressed ){        
        d_ptr->m_isPressed = false;
    }

    if ( d_ptr->m_mouseReleasePress ){        
        d_ptr->m_mouseReleasePress = false;
    }

    if ( d_ptr->m_cancelPressed ){
        d_ptr->m_cancelPressed = false;
    }
}

void PannableViewItem::pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event ){
    Q_UNUSED( event );
    if ( !d_ptr->m_acceptMouseEvent ){
        return;
    }

    // Doing the this with small delay we make possible that items are not selected
    // when user is panning the items.
    /*
    d_ptr->m_isPressed = true;
    QTimer::singleShot( 1500, this, SLOT( showMousePress() ) );
    */
}

void PannableViewItem::pannableViewMouseReleaseEvent( QGraphicsSceneMouseEvent * event ){    
    if ( !d_ptr->m_acceptMouseEvent ){
        return;
    }

    QRectF bBox             = boundingRect();
    QPointF mousePressPos   = event->buttonDownScenePos( Qt::LeftButton );
    QPointF mouseReleasePos = event->scenePos();

    if ( bBox.contains( mapFromScene( mouseReleasePos) ) &&
         bBox.contains( mapFromScene( mousePressPos ) ) ){
        //d_ptr->m_isPressed         = false;
        d_ptr->m_mouseReleasePress = true;
        update();        

        // Emit signal with small delay so that the selection effect is shown
        QTimer::singleShot( 500, this, SIGNAL( clicked() ) );        
    }
}

void PannableViewItem::pannableViewMouseMoveEvent( QGraphicsSceneMouseEvent * event ){
    if ( !d_ptr->m_acceptMouseEvent ){
        return;
    }

    QRectF bBox = boundingRect();
    QPointF mousePressPos   = event->buttonDownScenePos( Qt::LeftButton );
    QPointF mousePos        = event->scenePos();

    // Check if mouse has moved outside of this item. We don't want to highlight
    // the item in that case.
    /*
    if ( !bBox.contains( mapFromScene( mousePos ) ) ){
        d_ptr->m_cancelPressed = true;
        update();
    }
    */

}


void PannableViewItem::showMousePress(){    

    update();
}


void PannableViewItem::setShowSelection( bool show ){
    d_ptr->m_showSelection = show;
}

bool PannableViewItem::showSelection() const{
    return d_ptr->m_showSelection;
}

void PannableViewItem::setAcceptMouseEvent( bool accept){
    d_ptr->m_acceptMouseEvent = accept;
}

bool PannableViewItem::acceptMouseEvent() const{
    return d_ptr->m_acceptMouseEvent;
}
LabelItem * PannableViewItem::label() const{
    return d_ptr->m_label;
}

ImageItem * PannableViewItem::image() const{
    return d_ptr->m_image;
}

void PannableViewItem::refresh(){    
    layout()->activate();
}

}
