#ifndef HOME_HH
#define HOME_HH

#include <QGraphicsView>


namespace qtablet{

class HomePrivate;

class Home : public QGraphicsView
{
    Q_OBJECT
public:
    Home( QWidget * parent = 0 );

public slots:    
    void showOrHideWall();
private:
    HomePrivate * d_ptr;
};
}
#endif // HOME_H
