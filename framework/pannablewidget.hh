// TODO: Add GPL licence here

#ifndef PANNABLEWIDGET_HH
#define PANNABLEWIDGET_HH

#include <QGraphicsView>

class QGraphicsWidget;
class QWidget;
class PannableWidgetPrivate;


namespace qtablet{

  class PannableWidget: public QGraphicsView{
  Q_OBJECT
  public:
    PannableWidget( QWidget * parent = 0 );
    virtual ~PannableWidget();
    void setPannableWidget( QGraphicsWidget * widget );

  protected:        
    virtual void mousePressEvent( QMouseEvent * event );
    virtual void mouseReleaseEvent( QMouseEvent * event );
    virtual void mouseMoveEvent(  QMouseEvent * event );

  protected slots:
    void scroll( qreal value );

  private:
    PannableWidgetPrivate * d_ptr;
  };
}
#endif
