/* xwindow.h */


#ifndef XWINDOW_H
#define XWINDOW_H

#include <QGraphicsView>
#include <QPoint>
#include <QTimer>
#include <QPixmap>
#include <X11/X.h>

class QDesktopWidget;
class MouseEvent;
class QVBoxLayout;
class QGraphicsScene;
class XWindowToolbar;

class xwindow : public QGraphicsView
{
	Q_OBJECT

	QDesktopWidget *dt;         // root window
	WId clientid;               // client window
	int icx, icy, icw, ich;     // normal geometry in maximize state
	int scx, scy, scw, sch;     // normal geometry in screenmode
	int tcx, tcy, tcw, tch;     // normal geometry in tilemode
	char maxstate;              // maximized state 1=toolbar,2=screen
	QPoint mousepos;            // mouse position at button press	
	bool actpal;                // set to active colors
	bool urgpal;                // set to urgent colors
	QTimer *focustimer;         // autofocus timer
        //QTimer *tfocustimer;        // focus timer in tiled mode
	int borderh;                // diff between parent and client height
	int uborderh;               // upper border height;
	bool prot_delete;           // client has delete WM protocol
	bool prot_takefocus;        // client has take focus WM protocol
	int pos_x, pos_y;                       // XWMNormalHints 
	int base_w, base_h, inc_w, inc_h;       // XWMNormalHints
	int max_w, max_h, min_w, min_h;         // XWMNormalHints
	int wingrav;                            // XWMNormalhints
	long wmnflags;                          // XWMNormalHints
	uint init_w, init_h;        // window initial size
	bool withdrawnstate;
	bool unmapped;              // unmap called
	bool inputfield;            // WMHints
	bool map_iconic;            // WMHints
	bool urgent;                // WMHints
	Colormap cmap;              // colormap
	Window *cmapwins;           // array of subwindows with colormaps
	int ncmaps;                 // number of entries in cmaps
	bool shaped;                // nonrectangular window
	bool whidden;               // window in hidden state
	QString wmname;             // WM_NAME property or res_name
	QString icname;             // WM_ICON property or wmname
	QString res_name;           // ClassHint
	QString res_class;          // ClassHint
	QString clientmachine;      // WM_CLIENT_MACHINE
	QString clientfqdn;         // WM_CLIENT_MACHINE
	QString command;            // WM_COMMAND invoke line
	QString clientname;         // res_class or WM_COMMAND basename
	bool sstate;                // screen mode
	bool tstate;                // tile mode
	bool trsize;                // restore size after iconified in tiled mode
	Window transfor;            // TransientForHint or None
	int tnumber;                // numbering for window title
	QPixmap wicon;              // icon
	int pflags;                 // Flags from Property command
        //wframe *midmove;            // upper border mid frame for move
        //uborder *ubdr;              // upper window border
        //lborder *lbdr;              // lower window border
        QGraphicsScene  * scene;
        XWindowToolbar * toolbar;


	void getsize(int *, int *);            // adjust for possible size
	void send_wmprotocol(long, long);      // send WM_PROTOCOL to child window
        bool query_shape(void);                // nonrectangular window	
	int  get_clientstate(void);            // WM_STATE
	void get_servericon(Pixmap, Pixmap);   // set wicon to server icon pixmap
	void getwmprotocols(void);             // get wm protocols

        void createWindowDecoration();

public slots:
	void state(bool);           // map/unmap window
	void wdestroy(void);        // destroy client	
	void map(void);             // map/raise with check for map_iconic 
	void map_normal(void);      // map/raise without check for map_iconic
	void raise(void);           // raise only
	void show_info(void);       // show info on mid mouse button
	void unmap(void);
	void iconify(void);

	// window frame connects
	

	void s_maximize(void);      // toggle maximize to screen size
        void showHildonMenu();


public:
	xwindow(Window w, QWidget *parent=0);
	void get_wmname(void);          // get WM_NAME, set caption and title
	void set_title(void);           // set title on window frame
	void get_wmnormalhints(void);   // get WMNormalHints
	void get_wmhints(void);         // get WMHints
	void get_classhint(void);       // WMClassHint
	WId client_id(void)             { return(clientid); }
	int get_pflags(void)            { return(pflags); }
	QString getmachine(void)        { return(clientmachine); }
	void setinactive(void);         // set inactive colors
	void setactive(void);           // set active colors
	void seturgent(void);           // set urgent colors 
	void withdraw(void);            // transition to withdrawn state
	bool iswithdrawn(void)          { return(withdrawnstate); }
	int get_tnumber(void)           { return(tnumber); }
	QString get_clientname(void)    { return(clientname); }
	bool hidden_win(void)           { return(whidden); }
	void whide(void);               // hide window
	bool is_unmapped(void)          { return(unmapped); }
	bool is_tileable(void);         // client can be tiled
	bool is_tiled(void)             { return(tstate); }
	void minimize_frame(bool);      // minimize frame in tiling mode
	void focus_mouse(bool wlist=FALSE);     // map/raise and set mouse into window
	int  set_tile(int, int, int, int); // change to tiled desk
	void unset_tile(void);             // change to overlapped desk
	void reshape(void);             // reshape window
	void startautofocus(void);      // start autofocus
	void stopautofocus(void);       // stop autofocus timer
	void resize_client(int, int, int, int);   // resize with child window
	void resize_request(int, int, int, int);  // client resize request
	void unscreen(void);            // unset screenmode
	void set_clientstate(int);      // WM_STATE
	bool isshaped(void)             { return(shaped); }
	bool isstate(void)              { return(sstate); }
	void setchildfocus(long);       // set focus to child window
	void setcmapfocus(void);        // set colormap focus
	void setcmap(Colormap cm)       { cmap = cm; }
	void get_colormaps(void);       // get subwindow colormaps
	int get_clientflags(void);      // get client flags
	void set_pflags(int tflags=-1);     // change pflags
	int get_clienty(void)           { return(y()+uborderh); }
	WId gettransfor(void)           { return(transfor); }
	int getcheight(void)            { return(height()-borderh); } 
	QString ccaption(void)          { return(wmname); }
	QString icaption(void)          { return(icname); }
	QPixmap *icon(void)             { return(&wicon); }
	~xwindow(void);


};
#endif
