/* wborder.h */

#ifndef WBORDER_H
#define WBORDER_H

// define a frame that emits mouse events
#include <QLabel>
#include <QWidget>
#include <QString>

class QMouseEvent;
class QHBoxLayout;

class wframe : public QLabel
{
	Q_OBJECT
	
signals:
	void press(QMouseEvent *);
	void press(void);
	void release(QMouseEvent *);
	void mouse_move(QMouseEvent *);
	void left_press(QMouseEvent *);
	void left_press(void);
	void right_press(void);
	void left_release(QMouseEvent *);
	void right_release(void);
	void mid_press(void);
	void mid_release(void);

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);

public:
	wframe(QWidget *parent=0);
	void set_text(QString, bool);
};

// lower window border

class lborder : public QWidget
{
	QHBoxLayout *layout;

public:
	wframe *leftframe;
	wframe *midframe;
	wframe *rightframe;
	
	lborder(QWidget *parent=0);
};

// upper window border

class uborder : public QWidget
{
	QHBoxLayout *layout;

public:
	wframe *leftframe;
	wframe *midframe;
	wframe *rightframe;

	uborder(bool showleft=TRUE, QWidget *parent=0);
	void set_small(void);      // set to smaller frame
	void set_normal(void);     // set to normal frame
};
#endif
