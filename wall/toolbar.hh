#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "abstractitem.hh"

namespace qtablet{

class ToolbarPrivate;

class Pager;
class WindowList;
class ProcessBar;
class ApplicationBar;

class Toolbar : public AbstractItem
{
    Q_OBJECT
public:
    Toolbar( QGraphicsItem * parent = 0);

    virtual ~Toolbar();

    Pager * pager() const;

    WindowList *  windowList() const;

    ProcessBar * processBar() const;

    ApplicationBar * appicationBar() const;

    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const;

signals:
    void qlauncherButtonClicked();
    void pagerButtonClicked();

private:
    Q_DISABLE_COPY( Toolbar );
    ToolbarPrivate * d_ptr;
};

}
#endif // TOOLBAR_H
