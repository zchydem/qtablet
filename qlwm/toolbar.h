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

// The old toolbar
class Toolbar2 : public QFrame
{
    Q_OBJECT
public:
        QBoxLayout *layout;
        void addsep(void);              // add separator        
public:
        Toolbar2(QWidget *parent=0);
signals:
        void qButtonClicked();
public:
    pager         * tb_pg;
    winlist       * tb_wl;
    menu          * tb_mn;
    procbar       * tb_pb;
    apbar         * tb_ap;

};


class HomeDesktop: public QWidget
{
    Q_OBJECT
    public:
    static HomeDesktop * instance();

    private:
    HomeDesktop( QWidget * parent = 0 );
    public:
    pager   * getPager() const;
    winlist * getWinlist() const;
    menu    * getMenu() const;
    qtablet::Pager   * getProcbar() const;
    apbar   * getApbar() const;
    qtablet::Home * getHome() const;

    public slots:
    void qButtonClicked();
    void changeVirtualDesktop( int desktop );

    private:
    Toolbar2      * toolbar;
    qtablet::Home * home;    
    Q_DISABLE_COPY( HomeDesktop )
};

#endif
