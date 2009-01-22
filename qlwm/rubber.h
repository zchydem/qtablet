/* rubber.h */

#ifndef RUBBER_H
#define RUBBER_H
#include <X11/Xlib.h>

class rubber : public QLabel
{
	bool mpos;                 // show move position
	int px,py,pw,ph;           // frame
	XSegment segs[4];          // lines
	GC gc_thin,gc_thick;
	bool drawn;
	int base_w,base_h;         // XWMNormalHints
	int inc_w, inc_h;          // XWMNormalHints
	int old_x,old_y,old_w,old_h;

	void drawframe(void);

public:
	// upper constructor used for move
	// lower constructor used for resize
	
	rubber(QWidget *parent=0);
	rubber(int basew, int baseh, int incw, int inch, QWidget *parent=0);
	void init(void);
	void draw(int x, int y, int w, int h);  // draw frame
	~rubber(void);
};
#endif
