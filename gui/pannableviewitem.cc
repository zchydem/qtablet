#include "pannableviewitem.hh"
#include "imageitem.hh"
#include "labelitem.hh"

#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsLinearLayout>

namespace qtablet{

class PannableViewItemPrivate{
public:
    PannableViewItemPrivate():
            m_isPressed( false ),
            m_showSelection( true ),
            m_acceptMouseEvent( true ),
            m_image( new ImageItem ),
            m_label( new LabelItem )
    {

    }

    ~PannableViewItemPrivate(){
        if ( m_image ){
            delete m_image;
        }

        if ( m_label ){
            delete m_label;
        }
    }

    bool        m_isPressed;
    bool        m_showSelection;
    bool        m_acceptMouseEvent;
    ImageItem * m_image;
    LabelItem * m_label;
};


PannableViewItem::PannableViewItem(  QString const & imageFile, QString const & labelText, QGraphicsItem * parent ):
    AbstractItem( parent ),
    d_ptr( new PannableViewItemPrivate )
{
    // Make sure that child items don't get events i.e. they are behind this item
    d_ptr->m_image->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    d_ptr->m_label->setFlag( QGraphicsItem::ItemStacksBehindParent, true );

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
        QSizeF imageSize = d_ptr->m_image->sizeHint( Qt::MaximumSize );
        d_ptr->m_label->setWidth( static_cast<qint32>( imageSize.width() ) );
        d_ptr->m_label->setAlignment( Qt::AlignCenter );
        d_ptr->m_label->setFont( QFont( "Arial", 18, 68 ) );
        layout->addItem( d_ptr->m_label );
        layout->setAlignment( d_ptr->m_label, Qt::AlignCenter );
    }

    // And finally set the layout.
    setLayout( layout );
}


PannableViewItem::~PannableViewItem(){
    delete d_ptr;
}

void PannableViewItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ){
    Q_UNUSED( option );
    Q_UNUSED( widget );
    //qDebug() << "PannableViewItem::paint";

    if ( d_ptr->m_isPressed && d_ptr->m_showSelection ){
        painter->setPen( QPen( Qt::red, 4 ) );
        painter->drawRect( boundingRect() );
    }

    // Let's set flag to false here in order to enable fast tappings i.e. user
    // sees the selection of the item
    d_ptr->m_isPressed = false;
}

QSizeF PannableViewItem::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{    
    QGraphicsLinearLayout * l = static_cast<QGraphicsLinearLayout*>( layout() );
    if ( l){
        return l->sizeHint( which, constraint );
    }else{
        return QSizeF(0,0);
    }
}

void PannableViewItem::pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event ){
    Q_UNUSED( event );
    if ( !d_ptr->m_acceptMouseEvent ){
        return;
    }

    // Doing the this with small delay we make possible that items are not selected
    // when user is panning the items.
    d_ptr->m_isPressed = true;
    QTimer::singleShot( 1000, this, SLOT( update() ) );
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
        update();
        emit clicked();        
    }
}

void PannableViewItem::pannableViewMouseMoveEvent( QGraphicsSceneMouseEvent * event ){
    if ( !d_ptr->m_acceptMouseEvent ){
        return;
    }

    QRectF bBox = boundingRect();
    if ( !bBox.contains( this->mapFromScene( event->scenePos() ) ) ){
        d_ptr->m_isPressed = false;
        update();
    }
}


void PannableViewItem::update(){
    QGraphicsItem::update();
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

}
