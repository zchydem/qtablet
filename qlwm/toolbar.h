/* toolbar.h */
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QFrame>
#include <QWidget>

class pager;
class winlist;
class procbar;
class menu;
class apbar;
class QBoxLayout;


namespace qtablet{
    class Home;
    class Pager;
}


class HomeDesktop: public QWidget
{
    Q_OBJECT
    public:
    static HomeDesktop * instance();

    private:
    HomeDesktop( QWidget * parent = 0 );
    public:
    qtablet::Pager   * getPager() const;
    //qtablet::Pager   * getProcbar() const;
    qtablet::Home * getHome() const;

    public slots:
    void changeVirtualDesktop( int desktop );

    private:
    qtablet::Home * home;    
    Q_DISABLE_COPY( HomeDesktop )
};

#endif
