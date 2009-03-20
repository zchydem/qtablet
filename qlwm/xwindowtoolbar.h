#ifndef XWINDOWTOOLBAR_H
#define XWINDOWTOOLBAR_H

#include <QGraphicsWidget>

class XWindowToolbarPrivate;

//! XWIndowToolbar is a class for creating a toolbar above the client window.
//!
//!
class XWindowToolbar : public QGraphicsWidget
{
    Q_OBJECT
public:
    XWindowToolbar( bool transient, QGraphicsItem * parent = 0 );
    virtual ~XWindowToolbar();

    void setTitle( QString const & title );


signals:
    void minimizeWindow();
    void closeWindow();
    void showMenu();

private:
    Q_DISABLE_COPY( XWindowToolbar );
    XWindowToolbarPrivate * d_ptr;

};

#endif // XWINDOWTOOLBAR_H
