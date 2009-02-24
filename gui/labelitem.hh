#ifndef LABELITEM_H
#define LABELITEM_H

#include "abstractitem.hh"
namespace qtablet{

class LabelItemPrivate;

class LabelItem : public AbstractItem
{
    Q_OBJECT
public:
    LabelItem( QGraphicsItem * parent = 0 );

    LabelItem( QString const & text, QGraphicsItem * parent = 0 );

    virtual ~LabelItem();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );    

    virtual void setText( QString const & text );

    virtual void setFont( QFont const & font );

    virtual void setColor( QColor const & color );

    virtual void setAlignment( Qt::Alignment const & alignment );

    virtual void setWidth( quint32 width );

    virtual void clear();

    virtual QString const & text() const;

    virtual QFont  const & font() const ;

    virtual QColor const & color() const;

    virtual Qt::Alignment const & alignment() const;

    virtual bool isEmpty() const;

Q_SIGNALS:
    void clicked();

protected:
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:
    void updateLabel();

private:
    LabelItemPrivate * d_ptr;
    Q_DISABLE_COPY( LabelItem );
};

}
#endif // LABELITEM_H
