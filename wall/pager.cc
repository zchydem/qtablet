#include "pager.hh"
#include "pannableview.hh"
#include "labelitem.hh"
#include "imageitem.hh"


#include <QGraphicsLinearLayout>
#include <QPainter>
#include <QtDebug>
#include <QHash>

namespace qtablet{
class PagerPrivate{
public:
    PagerPrivate():
    m_currentDesktop(0),    
    m_size(800, 480){}

    qint32 m_currentDesktop;    
    QSizeF  m_size;
};

Pager::Pager(QGraphicsItem * parent ):
AbstractItem( parent ),
d_ptr( new PagerPrivate )
{
    d_ptr->m_currentDesktop = 0;
    setMinimumSize  ( d_ptr->m_size );
    setMaximumSize  ( d_ptr->m_size );
    setPreferredSize( d_ptr->m_size );

    QGraphicsLinearLayout * layout = new QGraphicsLinearLayout( Qt::Horizontal );
    for (int i=0; i < 2; i++){
        PagerDesktop * desktop = new PagerDesktop( i, tr("Desktop %1").arg(i), this );
        layout->insertItem( i,  desktop );
        connect( desktop, SIGNAL(changeDesktop(int)), this, SIGNAL(changeDesktop(int)) );

        // Default desktop is 0
        if (i == 0){
            desktop->highlight( true );
        }
    }

    setLayout( layout );    
}

Pager::~Pager()
{
    delete d_ptr;
    d_ptr = 0;
}


void Pager::setActiveDesktop( qint32 desktop ){
    QGraphicsLayout * currentLayout = layout();
    Q_CHECK_PTR( currentLayout );

    if ( currentLayout == 0 ){
        return;
    }

    for( qint32 i = 0; i < currentLayout->count(); i++ ){
        PagerDesktop * pd = static_cast<PagerDesktop*>(currentLayout->itemAt( i ));

        if ( pd == 0 ){
            continue;
        }

        if ( i == desktop ){
            pd->highlight( true );
            d_ptr->m_currentDesktop = desktop;
        }else{
            pd->highlight( false );
        }
    }
}


PagerDesktopItem * Pager::addWindow( qlonglong id, QString const & caption ){
   PagerDesktop * pd = static_cast<PagerDesktop*>(layout()->itemAt( d_ptr->m_currentDesktop ));
   PagerDesktopItem * item = pd->addWindow( id, caption );

   if ( item != 0 ){
       connect( item, SIGNAL(showWindow(bool)), this, SIGNAL(closePager()) );       
   }

   return item;
}

void Pager::removeWindow( qlonglong id ){

    //qDebug() << "TODO: implement Pager::removeWindow";
    PagerDesktop * pd = static_cast<PagerDesktop*>(layout()->itemAt( d_ptr->m_currentDesktop ));
    pd->removeWindow( id );
}

class PagerDesktopPrivate{
public:
    // Default ctor for initialization
    PagerDesktopPrivate():
            m_pannableView(0),
            m_desktopName(0),
            m_pannableLayout(0),
            m_highlight(false),
            m_windows(),
            m_size( 340, 460 ),
            m_id( -1 )
    {}



    // Members
    PannableView          * m_pannableView;
    LabelItem             * m_desktopName;
    QGraphicsLinearLayout * m_pannableLayout;
    bool                    m_highlight;
    QHash<qlonglong, PagerDesktopItem * > m_windows;
    QSizeF                  m_size;    
    int                     m_id;
};

PagerDesktop::PagerDesktop( int id, QString const & name, QGraphicsItem * parent ):
AbstractItem( parent ),
d_ptr( new PagerDesktopPrivate )
{

    setMinimumSize  ( d_ptr->m_size );
    setMaximumSize  ( d_ptr->m_size );
    setPreferredSize( d_ptr->m_size );

    // Initialize members and put them into a main layout
    d_ptr->m_pannableView   = new PannableView(Qt::Vertical, d_ptr->m_size.width() - 20 , d_ptr->m_size.height() - 70, this );
    d_ptr->m_desktopName    = new LabelItem(name, this );
    d_ptr->m_id             = id;

    d_ptr->m_desktopName->setFont( QFont("Arial", 28, 55 ) );
    d_ptr->m_pannableLayout = new QGraphicsLinearLayout( Qt::Vertical );
    d_ptr->m_pannableView->setLayout( d_ptr->m_pannableLayout );

    connect( d_ptr->m_desktopName, SIGNAL(clicked()), this, SLOT( changeDesktop() ) );

    //d_ptr->m_pannableLayout->setSpacing( 20 );
    qint32 margin = (minimumWidth() - 270) / 2;
    d_ptr->m_pannableLayout->setContentsMargins(margin, 20, margin, 20 );

    QGraphicsLinearLayout * layout = new QGraphicsLinearLayout(Qt::Vertical );
    layout->addItem( d_ptr->m_desktopName  );
    layout->addItem( d_ptr->m_pannableView );

    layout->setAlignment( d_ptr->m_desktopName, Qt::AlignCenter );
    layout->setAlignment( d_ptr->m_pannableView, Qt::AlignCenter | Qt::AlignTop );
    layout->activate();
    setLayout( layout );



}

PagerDesktop::~PagerDesktop(){
    delete d_ptr;
    d_ptr = 0;
}

QSizeF PagerDesktop::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return d_ptr->m_size;
}


void PagerDesktop::paint(QPainter *painter, const QStyleOptionGraphicsItem * const option, QWidget * widget ){
    // Draw nice background
    Q_UNUSED( option );
    Q_UNUSED( widget );

    QColor color;
    if ( d_ptr->m_highlight ){
        color = QColor(Qt::green);
    }else{
        color = QColor(Qt::lightGray );
    }

    color.setAlphaF(0.4f);
    painter->setBrush( color );
    painter->setPen(Qt::black);
    painter->drawRoundedRect( 0, 0, d_ptr->m_size.width(), d_ptr->m_size.height(), 15, 15 );
}

void PagerDesktop::highlight(bool highlight ){
    d_ptr->m_highlight = highlight;
    update();
}

PagerDesktopItem * PagerDesktop::addWindow( qlonglong id, QString const & caption ){

    bool windowExists = d_ptr->m_windows.contains( id );
    PagerDesktopItem * window = 0;

    // Get the pixmap of the window
    QPixmap pxm = QPixmap::grabWindow( id ).scaledToWidth( 270, Qt::SmoothTransformation );

    //qDebug() << "Screen capture size: " << pxm.size();

    if ( windowExists ){
        // Update the old item
        window = d_ptr->m_windows.value( id );
        window->updateImage( pxm );
    }else{
        // Create a new item
        window = new PagerDesktopItem( pxm, caption, this );
        d_ptr->m_windows.insert( id, window );      
        d_ptr->m_pannableLayout->addItem( window );
        d_ptr->m_pannableLayout->setAlignment( window, Qt::AlignCenter );
        connect( window, SIGNAL(ensureDesktop()), this, SLOT(changeDesktop()) );
    }

    qDebug() << "Adding/updating window:" << id << caption;

    d_ptr->m_pannableLayout->activate();
    d_ptr->m_pannableView->refresh();

    // Let's create a new layout and add all the items there
    if ( windowExists ){
        return 0;
    }else{
        return window;
    }
}

void PagerDesktop::removeWindow( qlonglong id ){


    if ( !d_ptr->m_windows.contains( id ) ){
        return;
    }

    //qDebug() << "removeWindow:" << id;
    PagerDesktopItem * item = 0;
    item = d_ptr->m_windows.value( id );

    //qDebug() << "Removing window: " << id << item->label()->text();

    qint32 removed = d_ptr->m_windows.remove( id );
    if ( removed != 1 ){
        qCritical() << "Failed to remove item with id: " << id;
        return;
    }

    d_ptr->m_pannableLayout->removeItem( item );
    item->hide();
    delete item;
    d_ptr->m_pannableLayout->activate();    
    d_ptr->m_pannableView->refresh();
}


void PagerDesktop::changeDesktop(){    
    qDebug() << "PagerDesktop::changeDesktop:" << d_ptr->m_id;
    Q_EMIT changeDesktop( d_ptr->m_id );
}


// Pager desktop item
PagerDesktopItem::PagerDesktopItem(  QPixmap const & pixmap, QString const & name, QGraphicsItem * parent ):
PannableViewItem(pixmap, name, parent ){    
    setFont( QFont("Arial", 20, 55 ) );
    label()->setWidth( pixmap.width() );
    label()->setAlignment( Qt::AlignCenter );
    connect( this, SIGNAL(clicked()), this, SLOT( showWindow() ) );
}

PagerDesktopItem::~PagerDesktopItem(){

}


void PagerDesktopItem::showWindow(){

    Q_EMIT ensureDesktop();
    Q_EMIT showWindow( true );
}

void PagerDesktopItem::updateImage( QPixmap const & pixmap ){ 
    image()->setImage( pixmap );    
    label()->setWidth( pixmap.width() );
    refresh();
    updateGeometry();
}


}
