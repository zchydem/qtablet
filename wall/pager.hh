#ifndef PAGER_HH
#define PAGER_HH

#include "abstractitem.hh"
#include "pannableviewitem.hh"



namespace qtablet{

class PagerDesktopItem;
class PagerPrivate;
class Pager : public AbstractItem
{
    Q_OBJECT
public:
    Pager(QGraphicsItem * parent = 0 );
    virtual ~Pager();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    //virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    void changeDesktop( qint32 desktop );

    PagerDesktopItem * addWindow( qlonglong id, QString const & caption );

    void removeWindow( qlonglong id );

    void removeAll(){}


signals:
    void setWindowState( bool map );
    void destroyWindow();

private:
    PagerPrivate * d_ptr;
    Q_DISABLE_COPY( Pager );
};

class PagerDesktopPrivate;
//! PagerDesktop is a graphical presentation of the one desktop in QTablet.
//! This widget contains 0 or more item depending if that desktop has any
//! clients.
class PagerDesktop: public AbstractItem
{
    Q_OBJECT
    public:
    PagerDesktop( QString const & name, QGraphicsItem * parent = 0 );
    virtual ~PagerDesktop();

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    //! Draw nice background
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem * const option, QWidget * widget );


    void highlight( bool highlight );
    PagerDesktopItem * addWindow( qlonglong id, QString const & caption );
    void removeWindow(qlonglong id );

    private:
    Q_DISABLE_COPY( PagerDesktop );
    PagerDesktopPrivate * d_ptr;
};


class PagerDesktopItem: public PannableViewItem
{
    Q_OBJECT
    public:

    PagerDesktopItem( QPixmap const & pixmap, QString const & name, QGraphicsItem * parent = 0 );

    virtual ~PagerDesktopItem();   

    virtual void updateImage( QPixmap const & pixmap );

    public slots:
    void showWindow();

    signals:

    void showWindow( bool show );

    void closeWindow();

    private:
    Q_DISABLE_COPY( PagerDesktopItem );
};

}
#endif // PAGER_H
