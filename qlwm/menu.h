/* menu.h */

#ifndef MENU_H
#define MENU_H
#include <QPushButton>
#include <QDateTime>
class QFile;
class QMenu;
class QAction;
class QMouseEvent;

class menu : public QPushButton
{
	Q_OBJECT
	
	QMenu *basemenu;
	bool killop;                  // window kill operation active
	QAction *act_quit;
	QAction *act_kill;
	QAction *act_rest;
	void run_cmd(QByteArray &);
	void winkill(void);
	void winkill_all(void);
	QDateTime lmtime;             // last file modification time
	static uchar kcursor_bits[];  // cursor bitmap
	static uchar kcursor_mask[];  // cursor bitmap

	void recurse(QMenu *, QFile &);
	
private slots:	
	void run_menu(QAction *);

protected:
	virtual void mousePressEvent(QMouseEvent *);

public:
	menu(QWidget *parent);
	void start_popup(void);
	void readmenu(void);
};
#endif
