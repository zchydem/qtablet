/* keyboard.h */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <X11/keysym.h>
#include "defs.h"

// key assignment
#define PREV_WINDOW        XK_Tab
#define PREV_WINDOW_MOD    (Mod1Mask | ShiftMask)

#define NEXT_WINDOW        XK_Tab
#define NEXT_WINDOW_MOD	   Mod1Mask

#define HIDDEN_WINDOW      XK_F12
#define HIDDEN_WINDOW_MOD  Mod1Mask

#define HIDE_WINDOW        XK_F9
#define HIDE_WINDOW_MOD    Mod1Mask

#define TOGGLE_SCREENMODE     XK_F11
#define TOGGLE_SCREENMODE_MOD Mod1Mask

#define TOGGLE_TILEDMODE      XK_F10
#define TOGGLE_TILEDMODE_MOD  Mod1Mask

#define PREV_DESK	   XK_Tab
#define PREV_DESK_MOD	   (ControlMask | ShiftMask)

#define NEXT_DESK          XK_Tab
#define NEXT_DESK_MOD	   ControlMask

#define WIN_POPUP          XK_Escape
#define WIN_POPUP_MOD	   ControlMask

#define MENU_POPUP         XK_Escape
#define MENU_POPUP_MOD	   Mod1Mask

class keyboard
{
	static uint NumLockMask;
	static void next_win(void);
	static void prev_win(void);
	static void hide_win(void);
	static void pdesk(void);
	static void ndesk(void);

public:
	static void init(void);
	static void tscreen(void);
	static bool keypress(XKeyEvent *);
};
#endif
