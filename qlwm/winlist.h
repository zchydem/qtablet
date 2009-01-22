/* winlist.h */

#ifndef WINLIST_H
#define WINLIST_H


#include <QPushButton>
#include <QIcon>
#include <X11/X.h>

class QMenu;
class QMouseEvent;
class xwindow;

class winlist : public QPushButton
{
	Q_OBJECT
	
	QMenu *wmenu;
	QIcon winlisticn,tiledicn;
	void popup_list(void);    // popup window list
	bool isobscured(xwindow *, Window *, uint);
	bool isbottom(xwindow *, Window *, uint);

private slots:
	void highlight_pager(QAction *);

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

public:
	void start_popup(void);
	void hidden_win(void);
	void set_pixmap(void);
	winlist(QWidget *parent);
};
#endif
