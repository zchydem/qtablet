// TODO: Add GPL licence here

#ifndef PANNABLEVIEW_HH
#define PANNABLEVIEW_HH

#include <QGraphicsView>

class QGraphicsWidget;
class QWidget;
class PannableViewPrivate;


namespace qtablet{
  //! PannableView is a simple view for the pannable content.
  //
  // PannableView provides quite simple event handling to enable
  // kinetic scrolling using touchscreen. It is only required to
  // create QGraphicsWidget with some content that is wanted to 
  // be panned.
  //
  // Example: Create layout with QPushButtons and set it to PannableView
  //
  // \code
  //
  // QGraphicsGridLayout * layout = new QGraphicsGridLayout;
  // layout->setSpacing( 20 );
  // qint32 row = 0;
  // qint32 col = 0;
  //
  // for ( int i = 0; i < 100; i++ ){
  //   QGraphicsProxyWidget *widget = new QGraphicsProxyWidget;
  //   widget->setWidget( new QPushButton( "Example Button" ) );
  //   layout->addItem( widget, row++%3, col );
  //   
  //   if ( row % 3 == 0 ){
  //      ++col;
  //   }
  //  }
  //
  // QGraphicsWidget form = new QGraphicsWidget;
  // form->setLayout( layout );
  // 
  // PannableView * pannableView = new PannableView;
  // pannableView->setPannableWidget( form );
  // \endcode
  //
  class PannableView: public QGraphicsView{
  Q_OBJECT
  public:
    //! PannableView constructor
    //
    //! \param orientation panning orientation i.e. vertical or horizontal
    //! \param parent optional parent    
    PannableView( Qt::Orientation orientation=Qt::Horizontal, QWidget * parent = 0 );

    //! Destructor
    virtual ~PannableView();

    //! Set pannable \p widget to this view.
    //
    //! \param widget widget to be panned.
    void setPannableWidget( QGraphicsWidget * widget );

  protected:        
    //! Event handler for mouse press event.
    //
    //! This method stops the scrolling and stores the mouse press position.
    //! Also the timer which is used for calculating the acceleration is reset
    //! in this method.
    //!
    //! \param event mouse event
    virtual void mousePressEvent( QMouseEvent * event );

    //! Event handler for mouse release event.
    //
    //! This method calculates the acceleration and starts the scrolling animation.
    //! \param event mouse release event
    virtual void mouseReleaseEvent( QMouseEvent * event );

    //! Event handler for mouse move event.
    //
    //! This method does the view translation if finger is moved slow enough i.e.
    //! the view follows the finger.
    //! \param event mouse move event
    virtual void mouseMoveEvent(  QMouseEvent * event );

  protected slots:
    //! Slot for animating the scrolling after fast finger swipe. 
    //
    //! This method is also responsible of calculating when one of the ends
    //! is reached and the scrolling must be stopped.
    void scroll( qreal value );

  private:
    PannableViewPrivate * d_ptr;
  };
}
#endif
