// TODO: Add GPL licence here

#ifndef PANNABLEVIEW_HH
#define PANNABLEVIEW_HH

#include <QGraphicsWidget>
#include <QPainterPath>


class QWidget;
class PannableViewPrivate;
class PannableWidgetPrivate;

namespace qtablet{

    //! PannableView is a view for the pannable content.
    //
    // PannableView provides convenient interface for using PannableWidget.
    // It is recommended to use this class instead of using PannableWidget,
    // because this class provides a view to the specified area in PannableWidget.
    //
    //
    //
    // Example:
    //
    // \code
    // QGraphicsScene scene;
    // qtablet::PannableView * viewport = new qtablet::PannableView( Qt::Horizontal, 600, 400 );
    // viewport->setLayout( createSomeQGraphicsLayout() );
    // scene.addItem( viewport );
    // viewport->setPos( 100, 40 )
    // \endcode
    //
    class PannableView: public QGraphicsWidget{
        Q_OBJECT
    public:
        //! Constructor. Create PannableView object, with specified properties.
        // \param orientation Qt::Horizontal or Qt::Vertical
        // \param width The width of the view
        // \param height The height of the view
        // \param parent optional parent
        PannableView( Qt::Orientation orientation, qreal width, qreal height, QGraphicsItem * parent = 0 );

        //! Destructor
        virtual ~PannableView();

        //! Set the layout which is pannable to this view.
        void setLayout( QGraphicsLayout * layout );

    private:
        PannableViewPrivate * d_ptr;
    };

  //! PannableWidget is a simple widget for the pannable content.
  //
  // PannableWidget provides quite simple event handling to enable
  // kinetic scrolling using touchscreen. It is only required to
  // create QGraphicsWidget with some content that is wanted to 
  // be panned.
  //
  // Example: Create layout with QPushButtons and set it to PannableWidget
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
  // PannableWidget * pannableView = new PannableWidget;
  // pannableView->setPannableWidget( form );
  // \endcode
  //
  class PannableWidget: public QGraphicsWidget{
  Q_OBJECT
  public:
    //! PannableWidget constructor
    //
    //! \param orientation panning orientation i.e. vertical or horizontal
    //! \param parent optional parent    
    PannableWidget( Qt::Orientation orientation=Qt::Horizontal, QGraphicsItem * parent = 0 );

    //! Destructor
    virtual ~PannableWidget();


    virtual QPainterPath shape () const;


  protected:        
    //! Event handler for mouse press event.
    //
    //! This method stops the scrolling and stores the mouse press position.
    //! Also the timer which is used for calculating the acceleration is reset
    //! in this method.
    //!
    //! \param event mouse event
    virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );

    //! Event handler for mouse release event.
    //
    //! This method calculates the acceleration and starts the scrolling animation.
    //! \param event mouse release event
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );

    //! Event handler for mouse move event.
    //
    //! This method does the view translation if finger is moved slow enough i.e.
    //! the view follows the finger.
    //! \param event mouse move event
    virtual void mouseMoveEvent(  QGraphicsSceneMouseEvent * event );


  protected slots:
    //! Slot for animating the scrolling after fast finger swipe. 
    //
    //! This method is also responsible of calculating when one of the ends
    //! is reached and the scrolling must be stopped.
    void scroll( qreal value );

  private:
    PannableWidgetPrivate * d_ptr;
  };
}
#endif
