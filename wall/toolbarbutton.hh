#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H

#include "abstractitem.hh"

namespace qtablet{

class ToolbarButtonPrivate;

class ToolbarButton : public AbstractItem
{
    Q_OBJECT
public:

    enum ButtonType{
        QLauncherButton,
        QPagerButton
    };

    ToolbarButton( ButtonType type, QGraphicsItem * parent = 0);

    virtual ~ToolbarButton();

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem * const option, QWidget * widget );

protected:
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

signals:
    void clicked();

private:
    Q_DISABLE_COPY( ToolbarButton );
    ToolbarButtonPrivate * d_ptr;
};
}
#endif // TOOLBARBUTTON_H
