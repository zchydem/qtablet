#ifndef HOME_HH
#define HOME_HH

#include <QGraphicsView>


namespace qtablet{


class Pager;
class HomePrivate;

class Home : public QGraphicsView
{
    Q_OBJECT
public:
    Home( QWidget * parent = 0 );

    Pager * pager() const;

public slots:    
    void showOrHideWall();
    void showOrHidePager();
private:
    HomePrivate * d_ptr;
};
}
#endif // HOME_H
