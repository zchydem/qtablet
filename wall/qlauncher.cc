#include "qlauncher.hh"
#include "pannableview.hh"
#include "pannableviewitem.hh"
#include "settings.hh"
#include "labelitem.hh"
#include "desktopfileparser.hh"

#include <QtDebug>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QPainter>


namespace qtablet{

class QLauncherPrivate{
public:
    QLauncherPrivate():
        m_pannableView(0),
        m_header      (0),
        m_size        ( )
    {
    }

    PannableView * m_pannableView;
    LabelItem    * m_header;
    QSizeF         m_size;
    QPixmap        m_background;
};


QLauncher::QLauncher( QGraphicsItem * parent ):
    AbstractItem( parent ),
    d_ptr( new QLauncherPrivate )
{
    d_ptr->m_size         = QSizeF( Settings::instance()->screenResolution() );
    d_ptr->m_pannableView = new PannableView( Qt::Horizontal, d_ptr->m_size.width(), d_ptr->m_size.height() - 50, this );
    d_ptr->m_header       = new LabelItem( trUtf8("QLauncher"), this );
    d_ptr->m_header->setFont( QFont( "Arial", 28, 68 ) );    
    d_ptr->m_header->setColor(Qt::gray );

    // TODO: Read default icon name from the qtablet.conf
    // Set background    
    d_ptr->m_background.load("image:launcher_bg1");


    QGraphicsLinearLayout * mainLayout = new QGraphicsLinearLayout( Qt::Vertical );
    mainLayout->setSpacing( 20 );
    mainLayout->addItem( d_ptr->m_header );
    mainLayout->addItem( d_ptr->m_pannableView );

    setLayout( mainLayout );
    createLauncherItems();
}

QLauncher::~QLauncher(){
    delete d_ptr->m_pannableView;
    delete d_ptr->m_header;
    delete d_ptr;
}

void QLauncher::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ){
    Q_UNUSED( option );
    Q_UNUSED( widget );
    if ( d_ptr->m_background.isNull() ){
        qCritical() << "QLauncher: NULL background image. Can't draw the background for the launcher!";
    }else{
        painter->drawPixmap( 0, 0, d_ptr->m_background );
    }
}

QSizeF QLauncher::sizeHint ( Qt::SizeHint which, const QSizeF & constraint ) const{
    Q_UNUSED( which );
    Q_UNUSED( constraint );
    return d_ptr->m_size;
}

void QLauncher::createLauncherItems(){

    // Layout for items
    // TODO: REMOVE HARDCODED VALUES
    QGraphicsGridLayout * layout = new QGraphicsGridLayout;    
    layout->setRowFixedHeight( 0, 190 );
    layout->setRowFixedHeight( 1, 190 );
    layout->setSpacing( 30 );

    quint32 row = 0;
    quint32 col = 0;

    QStringList desktopFiles = Settings::instance()->desktopFiles();

    // Parse each desktop file
    foreach( QString file, desktopFiles ){

        QMap<QString, QString> content = DesktopFileParser::parse( file );

        // Create PannableViewItem
        QPixmap pxm( "icon:" + content.value( DesktopFileParser::Icon ) );
        QString name = content.value( DesktopFileParser::Name );

        PannableViewItem * item = new PannableViewItem( pxm, name );

        layout->addItem( item, row % 2, col, Qt::AlignCenter );
        layout->setAlignment( item, Qt::AlignCenter | Qt::AlignHCenter );

        if ( row % 2 == 1 ){
            ++col;
        }
        ++row;

        // TODO: Add other stuff from content to attributes of the item
        item->addItemAttribute( DesktopFileParser::Encoding,   content.value( DesktopFileParser::Encoding   ) );
        item->addItemAttribute( DesktopFileParser::Version,    content.value( DesktopFileParser::Version    ) );
        item->addItemAttribute( DesktopFileParser::Type,       content.value( DesktopFileParser::Type       ) );
        item->addItemAttribute( DesktopFileParser::Name,       content.value( DesktopFileParser::Name       ) );
        item->addItemAttribute( DesktopFileParser::Exec,       content.value( DesktopFileParser::Exec       ) );
        item->addItemAttribute( DesktopFileParser::Icon,       content.value( DesktopFileParser::Icon       ) );
        item->addItemAttribute( DesktopFileParser::MimeType,   content.value( DesktopFileParser::MimeType   ) );
        item->addItemAttribute( DesktopFileParser::Comment,    content.value( DesktopFileParser::Comment    ) );
        item->addItemAttribute( DesktopFileParser::Categories, content.value( DesktopFileParser::Categories ) );

        // TODO: Handle clicked signals from the items.
        connect( item, SIGNAL( clicked() ), this, SLOT( execApplication() ) );
    }

    d_ptr->m_pannableView->setLayout( layout );
}

void QLauncher::execApplication(){
    PannableViewItem * item = static_cast<PannableViewItem*>( sender() );
    if ( item == 0 ){
        qCritical() << "Failed to get signal emitter item";
        return;
    }

    qDebug() << "TODO: execApplication not implemented yet."
             << "exec: " << item->itemAttribute( DesktopFileParser::Exec ).toString();
}

void QLauncher::showApplicationInfo(){
    // THIS REQUIRES NEW SIGNAL FOR THE PannableViewItem
    qDebug() << "TODO: showApplicationInfo not implemented yet";
}

}
