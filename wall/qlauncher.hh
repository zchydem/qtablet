#ifndef QLAUNCHER_H
#define QLAUNCHER_H

#include "abstractitem.hh"

namespace qtablet{

class QLauncherPrivate;

class QLauncher : public AbstractItem
{
    Q_OBJECT
public:
    QLauncher( QGraphicsItem * parent = 0 );

    virtual ~QLauncher();

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

protected:
    virtual void createLauncherItems();

protected slots:
    void execApplication();
    void showApplicationInfo();

private:
    QLauncherPrivate * d_ptr;
};

}
#endif // QLAUNCHER_H
