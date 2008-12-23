#ifndef MAINWALL_HH
#define MAINWALL_HH

#include <QWidget>
class QtAnimation;
class QtState;

namespace qtablet{

  class MainWall: public QWidget{
    Q_OBJECT
    public:

    MainWall( QWidget * parent );
    virtual ~MainWall();
  private slots:
    void animate();

  private:
    QtAnimation * anim1;
    QtAnimation * anim2;
    QtState *state1;
    QtState *state2;
  };
}

#endif
