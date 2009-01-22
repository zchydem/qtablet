/* keyboard.cpp */

#include "keyboard.h"
#include "qapp.h"
#include "defaults.h"

uint keyboard::NumLockMask;

void keyboard::init(void)
{
	int i;
	static KeySym keys[] = { PREV_WINDOW,NEXT_WINDOW,HIDDEN_WINDOW,HIDE_WINDOW,
	TOGGLE_SCREENMODE,TOGGLE_TILEDMODE,PREV_DESK,NEXT_DESK,WIN_POPUP,MENU_POPUP };
	static KeyCode mods[] = { PREV_WINDOW_MOD,NEXT_WINDOW_MOD,HIDDEN_WINDOW_MOD,HIDE_WINDOW_MOD,
	TOGGLE_SCREENMODE_MOD,TOGGLE_TILEDMODE_MOD,PREV_DESK_MOD,NEXT_DESK_MOD,WIN_POPUP_MOD,MENU_POPUP_MOD };
 	XModifierKeymap* xmk = XGetModifierMapping(QX11Info::display());
	
        for(i=0; i < 8; i++)
	{
		if(xmk->modifiermap[xmk->max_keypermod * i] == XKeysymToKeycode(QX11Info::display(), XK_Num_Lock))
		{
		         NumLockMask = (1<<i);
			 break;
		}	 
	}

	for(i=0; i < 10; i++)
	{
		XGrabKey(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), keys[i]), mods[i], QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
		XGrabKey(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), keys[i]), mods[i]|LockMask, QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
		XGrabKey(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), keys[i]), mods[i]|NumLockMask, QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
		XGrabKey(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), keys[i]), mods[i]|LockMask|NumLockMask, QX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync);
	}
}

bool keyboard::keypress(XKeyEvent *kev)
{
	if(qapp::menu_open)
		return FALSE;
		
	KeySym sym = (int)XLookupKeysym(kev, 0);
	uint mod = kev->state & ~(NumLockMask|LockMask);
        HomeDesktop * home = HomeDesktop::instance();

	if(sym == PREV_WINDOW && mod == PREV_WINDOW_MOD) {
	    prev_win();
	    return TRUE;
	} 

	if(sym == NEXT_WINDOW && mod == NEXT_WINDOW_MOD) {
	    next_win();
	    return TRUE;
	}

	if(sym == HIDDEN_WINDOW && mod == HIDDEN_WINDOW_MOD) {
            home->getWinlist()->hidden_win();
	    return TRUE;
	}

	if(sym == HIDE_WINDOW && mod == HIDE_WINDOW_MOD) {
	    hide_win();
	    return TRUE;
	}

	if(sym == TOGGLE_SCREENMODE && mod == TOGGLE_SCREENMODE_MOD) {
	    tscreen();
	    return TRUE;
	}

	if(sym == TOGGLE_TILEDMODE && mod == TOGGLE_TILEDMODE_MOD) {
	    qapp::toggle_tiled();
	    return TRUE;
	}

	if(sym == PREV_DESK && mod == PREV_DESK_MOD) {
	    pdesk();
	    return TRUE;
	}

	if(sym == NEXT_DESK && mod == NEXT_DESK_MOD) {
	    ndesk();
	    return TRUE;
	}

	if(sym == WIN_POPUP && mod == WIN_POPUP_MOD) {
            home->getWinlist()->start_popup();
	    return TRUE;
	}

	if(sym == MENU_POPUP && mod == MENU_POPUP_MOD) {	  
            home->getMenu()->start_popup();
	    return TRUE;
	}

	return FALSE;
}

void keyboard::prev_win(void)
{
	xwindow *cur;
	int curcl;

	if(clients.size() < 2)
		return;
		
	if((curcl = clients.indexOf(qapp::focusclient)) == -1)
		curcl = clients.size()-1;

	for(int i=0; i <= clients.size(); i++)
	{
		curcl--;
		if(curcl < 0)
			curcl = clients.size()-1;
			
		cur = clients.at(curcl);
			
		if(cur->isVisible() && ! (qapp::smode && (cur->isshaped() || (cur->get_pflags() & qapp::NoScreen))) &&
		! (cur->get_pflags() & qapp::NoKey) && (qapp::smode || ! (cur->get_pflags() & qapp::StayOnBottom)) && qapp::is_curdesk(cur))
		{
			cur->focus_mouse();
			return;
		}
	} 
}

void keyboard::next_win(void)
{
	xwindow *cur;
	int curcl;

	if(clients.size() < 2)
		return;

	if((curcl = clients.indexOf(qapp::focusclient)) == -1) 
		curcl = 0;

	for(int i=0; i <= clients.size(); i++)
	{
		curcl++;
		if(curcl >= clients.size())
			curcl = 0;
			
		cur = clients.at(curcl);

		if(cur->isVisible() && ! (qapp::smode && (cur->isshaped() || (cur->get_pflags() & qapp::NoScreen))) &&
		! (cur->get_pflags() & qapp::NoKey) && (qapp::smode || ! (cur->get_pflags() & qapp::StayOnBottom)) && qapp::is_curdesk(cur))
		{
			cur->focus_mouse();
			return;
		}
	} 
}

void keyboard::hide_win(void)
{
	if(qapp::smode)
		return;

	if(clients.indexOf(qapp::focusclient) != -1 && qapp::focusclient->isVisible())
		qapp::focusclient->whide();
}

void keyboard::tscreen(void)
{
	xwindow *client,*cur;
	int i,curcl;
	
	if(qapp::smode)
	{
		qapp::smode = FALSE;
		qapp::stopautofocus();

		if(clients.size() == 0)
			return;

		for(i=0; i < clients.size(); i++)
			clients.at(i)->unscreen();

		if(qapp::is_tileddesk())
			qapp::tile_order(qapp::focusclient);
	}
	else
	{
		if(clients.size() == 0 || qapp::focusclient == NULL)
			return;

		if((curcl = clients.indexOf(qapp::focusclient)) == -1)
			curcl = 0;

		client = cur = clients.at(curcl);

		do
		{
			if(cur->isVisible() && ! (qapp::smode && (cur->isshaped() || (cur->get_pflags() & qapp::NoScreen))) &&
			! (cur->get_pflags() & qapp::NoKey) && qapp::is_curdesk(cur))
			{
				qapp::smode = TRUE;
				qapp::stopautofocus();
				cur->focus_mouse();
				return;
			}
			if(++curcl >= clients.size())
				curcl = 0;
			cur = clients.at(curcl);
		}
		while(cur != client);
	}
}

void keyboard::pdesk(void)
{
	if(qapp::smode)
		return;
		
	int desk = qapp::adesk;

	if(--desk < 0)
		desk = defaults::vdesks-1;

        HomeDesktop::instance()->getPager()->change_desk(desk);
}

void keyboard::ndesk(void)
{
	if(qapp::smode)
		return;

	int desk = qapp::adesk;

	if(++desk >= defaults::vdesks)
		desk = 0;

        HomeDesktop::instance()->getPager()->change_desk(desk);
}
