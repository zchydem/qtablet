/* pager.h */

#ifndef PAGER_H
#define PAGER_H



#include <QLabel>
class QPixmap;
class xwindow;
class pager : public QLabel
{
	int vdesks;         // number of virtual desks
	int dwidth;         // desktop width
	int dheight;        // desktop height
	int vw;             // virtual desk width
	int vh;             // virtual desk height
	QPixmap *backpix;   // background drawable

	void draw_window(QPainter *painter, QRect &crect, bool hlight=FALSE);

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void paletteChange(const QPalette &);

public:
	pager(QWidget *parent);
	void draw_pager(void);                   // redraw pager
	void change_desk(int desk);              // change active desktop and redraw
	void add(xwindow *, bool hlight=FALSE);  // add window
};
#endif
