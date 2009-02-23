#ifndef PANNABLEVIEWITEM_HH
#define PANNABLEVIEWITEM_HH

#include "abstractitem.hh"
namespace qtablet{

class PannableViewItemPrivate;
class LabelItem;
class ImageItem;

class PannableViewItem : public AbstractItem
{
    Q_OBJECT
public:
    PannableViewItem( QString const & imageFile, QString const & labelText=QString(), QGraphicsItem * parent = 0 );

    PannableViewItem( QPixmap const & pixmap, QString const & labelText=QString(), QGraphicsItem * parent = 0 );

    virtual ~PannableViewItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual void pannableViewMousePressEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMouseReleaseEvent( QGraphicsSceneMouseEvent * event );

    virtual void pannableViewMouseMoveEvent( QGraphicsSceneMouseEvent * event );

    virtual void setShowSelection( bool show );

    virtual bool showSelection() const;

    virtual void setAcceptMouseEvent( bool accept );

    virtual bool acceptMouseEvent() const;

    virtual LabelItem * label() const;

    virtual ImageItem * image() const;

    virtual void refresh();


signals:
    void clicked();

protected slots:
    void showMousePress();

private:
    PannableViewItemPrivate * d_ptr;
    Q_DISABLE_COPY( PannableViewItem );
};
}
#endif // PANNABLEVIEWITEM_H
