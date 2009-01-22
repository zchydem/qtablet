/*
*  File      : procbar.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Creates the buttons for iconified windows
*/

#include "defs.h"
#include "defaults.h"
#include "procbar.h"

// max. button width
#define max_bwidth 200

procbar::procbar(QWidget *parent) : QWidget(parent)
{
	setFont(defaults::toolbarfont);
}

void procbar::change_text(QString *icname, xwindow *win)
{	
	WINLIST *w;
	int i;
	
	for(i=0; i < wlist.size(); i++)
		if(wlist.at(i)->win == win)
			break;
	
	if(i == wlist.size())
		return;
	
	w = wlist.at(i);
	w->button->setText(*icname);

	if(win->icon() != NULL)
	{
		QIcon icon(*win->icon());
		w->button->setIcon(icon);
	}	
}

bool procbar::remove_fg(void)   // return FALSE if nothing to delete
{
	WINLIST *w;
	
	for(int i=0; i < wlist.size(); i++)
	{
		w = wlist.at(i);
		if(w->win->isVisible())
		{
			delete w->button;
			wlist.removeAt(i);
			
			return(TRUE);
		}
	}
	return(FALSE);
}

void procbar::draw_buttons(void)
{
	WINLIST *w;
	int i,i2;
	
	if(wlist.isEmpty())
		return;
	
	int h = height();
	int pos = 0;     // current position
	bwidth = (width()/wlist.size())-1;   // button width

	if(bwidth < h)         // scroll on overflow
	{
		bwidth = h;

		int tsize = wlist.size()*(h+1);
		int maxscr = ((tsize-width())/(h+1))+1;
		w = wlist.at(i=0);
		for(i2=0; i2 < maxscr; i2++)
		{
			w->button->hide();
			w = wlist.at(++i);
		}
	}
	else
	{
		i=0;
		if(bwidth > max_bwidth)
			bwidth = max_bwidth-1;
	}

        //QVBoxLayout * layout = new QVBoxLayout(this);
 	while(i < wlist.size())
	{
		w = wlist.at(i);
                w->button->setGeometry(pos, 0, bwidth, h);
                //w->button->resize( 70, 50 );
                //layout->addWidget( w->button );

		if(! w->button->isVisible())
			w->button->show();
		
		pos += bwidth+1;
		i++;
	}

        //layout->activate();
        //setLayout( layout );
}

void procbar::resizeEvent(QResizeEvent *)
{
	draw_buttons();
}

void procbar::add(xwindow *win)
{
	WINLIST *w;
	int i;
	
	if(win == NULL)
		return;

	foreach(w, wlist)
	{
		if(w->win == win)    // window already on bar
		{
			w->button->setChecked(FALSE);
			return;
		}
	}
	i = wlist.size(); 
	while(i--*max_bwidth > width() && remove_fg()) ;
	
	w = new WINLIST;
	Q_CHECK_PTR(w);
	w->button = new QPushButton(this);        
	Q_CHECK_PTR(w->button);
	w->button->lower();

	if(win->icon() != NULL)
		w->button->setIcon(QIcon(*win->icon()));

	w->button->setText(win->icaption());
	w->win = win;
	w->button->setCheckable(TRUE);
	connect(w->button, SIGNAL(toggled(bool)), win, SLOT(state(bool)));

	wlist.append(w);
	draw_buttons();
}

void procbar::set_on(xwindow *win)
{
	WINLIST *w;
	
	foreach(w, wlist)
	{
		if(w->win == win)    // window on bar
		{
			w->button->setChecked(TRUE);
			break;
		}
	}
	return;
}	

void procbar::change_palette(const QPalette &bpal, xwindow *win)
{
	int i;
	
	for(i=0; i < wlist.size(); i++)
		if(wlist.at(i)->win == win)
			break;
	
	if(i == wlist.size())
		return;
	
	wlist.at(i)->button->setPalette(bpal);
}

void procbar::remove(xwindow *win)   // remove from bar
{
	WINLIST *w;
	int i;

	for(i=0; i < wlist.size() && wlist.at(i)->win != win; i++) ;

	if(i == wlist.size())
		return;
	
	w = wlist.takeAt(i);
	delete w->button;
	delete w;

	draw_buttons();
}

void procbar::remove_all(void)
{
	WINLIST *w;

	foreach(w, wlist)
	{
		delete w->button;
		delete w;
	}
	wlist.clear();
}
