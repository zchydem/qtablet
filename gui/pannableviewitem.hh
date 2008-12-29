#ifndef PANNABLEVIEWITEM_HH
#define PANNABLEVIEWITEM_HH

#include "abstractitem.hh"
namespace qtablet{

class PannableViewItemPrivate;

class PannableViewItem : public AbstractItem
{
    Q_OBJECT
public:
    PannableViewItem( QGraphicsItem * parent = 0 );

    virtual ~PannableViewItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual void pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMouseReleaseEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMousePressCancelled();

protected:
    virtual void timerEvent( QTimerEvent * event );

signals:
    void clicked();

private:
    PannableViewItemPrivate * d_ptr;
    Q_DISABLE_COPY( PannableViewItem );
};
}
#endif // PANNABLEVIEWITEM_H
