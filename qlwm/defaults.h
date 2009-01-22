/* defaults.h */

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "xwindow.h"

class defaults
{

public:
	static int tb_height;
	static int tc_height;
	static int vdesks;
	static bool toolbar_top;
	static int lowerborderwidth;
	static int lowerborderheight;
	static int windowbuttonsize;
	static QFont borderfont;
	static QFont toolbarfont;
	static QColor *inactive_bg;
	static QColor urgent_bg;
	static QColor root_bg;
	static QString root_pix;
	static bool starturgent;
	static QStack <QByteArray *> initexec;
	static bool show_menu;
	static bool show_winlist;
	static bool showclientmachines;
	static int autofocustime;
	static bool start_restart;
	static char **argv;
	static int argc;
	static float tleftspace;
	static int maxontab;
	static int wminframe;
	static bool sttiled[10];
	static QString cfdir;
	static int tmx1,tmx2,tmy1,tmy2;
	static int smx1,smx2,smy1,smy2;
	static int pager_height;
	static QColor pager_active;
	static QColor pager_window;
	static QColor pager_visible;
	
	static void read_config(void);

};
#endif
