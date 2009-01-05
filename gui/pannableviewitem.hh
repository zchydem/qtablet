#ifndef PANNABLEVIEWITEM_HH
#define PANNABLEVIEWITEM_HH

#include "abstractitem.hh"
namespace qtablet{

class PannableViewItemPrivate;

class PannableViewItem : public AbstractItem
{
    Q_OBJECT
public:
    PannableViewItem( QString const & imageFile, QString const & labelText=QString(), QGraphicsItem * parent = 0 );

    virtual ~PannableViewItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    virtual void pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMouseReleaseEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMouseMoveEvent( QGraphicsSceneMouseEvent * event );

    virtual void setShowSelection( bool show );

    virtual bool showSelection() const;

    virtual void setAcceptMouseEvent( bool accept );

    virtual bool acceptMouseEvent() const;

signals:
    void clicked();

protected slots:
    void update();

private:
    PannableViewItemPrivate * d_ptr;
    Q_DISABLE_COPY( PannableViewItem );
};
}
#endif // PANNABLEVIEWITEM_H
