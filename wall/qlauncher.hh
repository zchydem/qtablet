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


protected:
    virtual void createLauncherItems();

protected slots:
    void execApplication();
    void showApplicationInfo();

signals:
    void hideQLauncher();

private:
    QLauncherPrivate * d_ptr;
};

}
#endif // QLAUNCHER_H
