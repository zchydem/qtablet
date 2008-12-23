#ifndef QLAUNCHER_HH
#define QLAUNCHER_HH

#include <QGraphicsWidget>


namespace qtablet{

  class PannableView;

  class QLauncher: public QGraphicsWidget{
    Q_OBJECT
    public:

    QLauncher( QGraphicsWidget * parent = 0 );
    virtual ~QLauncher();

  private:
    PannableView * pannableView;

  };
}
#endif
