#ifndef WALL_HH
#define WALL_HH

#include "abstractitem.hh"

namespace qtablet{

class Wall : public AbstractItem
{
    Q_OBJECT
public:
    Wall( QGraphicsItem * parent = 0 );
    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;
};

}
#endif // WALL_H
