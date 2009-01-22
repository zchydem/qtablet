/*
*  File      : defaults.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  reads the defaults file and makes information statically
*  accessible
*/

#include <QStyleFactory> 

#include "defs.h"
#include "conf.h"
#include "qapp.h"
#include "defaults.h"

int defaults::tb_height = 24;                   // toolbar height
int defaults::tc_height = 20;                   // toolbar contents height
int defaults::vdesks = 3;                       // virtual desktops
bool defaults::toolbar_top = FALSE;             // toolbar on top
int defaults::lowerborderwidth = 22;            // lower window border
int defaults::lowerborderheight = 6;            // lower window border
int defaults::windowbuttonsize = 16;            // upper window border
QFont defaults::borderfont;                     // window border font
QFont defaults::toolbarfont;                    // toolbar font
QColor *defaults::inactive_bg = NULL;           // window inactive color
QColor defaults::urgent_bg = Qt::darkMagenta;   // window urgent color
QColor defaults::root_bg = Qt::black;           // root window background
QString defaults::root_pix;                     // root window background pixmap
bool defaults::starturgent = TRUE;              // new windows with urgent colors
QStack <QByteArray *> defaults::initexec;       // Loaded with Exec option
bool defaults::showclientmachines = FALSE;      // show client machine names on winlist
bool defaults::show_menu = TRUE;                // show menu on toolbar
bool defaults::show_winlist = TRUE;             // show winlist on toolbar
int defaults::autofocustime = 600;              // autofocus time/ms or 0 = off
bool defaults::start_restart = FALSE;           // restart running
char **defaults::argv;                          // copy for restart
int defaults::argc;                             // copy for restart
float defaults::tleftspace = 0.65;              // left frame in tiled desk
int defaults::maxontab = 600;                   // maximize with tab key in tiled mode
int defaults::wminframe = 3;                    // minimize right frame in tiling mode
bool defaults::sttiled[10];                     // deskops to be tiled after start
QString defaults::cfdir = NULL;                 // config directory
int defaults::tmx1=-1,defaults::tmx2,defaults::tmy1,defaults::tmy2; 
int defaults::smx1=-1,defaults::smx2,defaults::smy1,defaults::smy2;
int defaults::pager_height = 18;                // pager height
QColor defaults::pager_active = Qt::darkGray;   // active background color
QColor defaults::pager_window = Qt::blue;       // window background color
QColor defaults::pager_visible = Qt::darkRed;   // visible window background color

void defaults::read_config(void)
{
	int ival,i,i2,iar[4];
	float fval;
	bool ok;

	borderfont.setPixelSize(windowbuttonsize-5);

	static const char *par[] = {
	"ToolbarHeight",          // 0
	"WindowButtonSize",       // 1
	"LowerBorderHeight",      // 2
	"LowerBorderWidth",       // 3
	"PagerHeight",            // 4
	"WindowFontName",         // 5
	"WindowFontHeight",       // 6
	"IconFontName",           // 7
	"IconFontHeight",         // 8
	"ToolbarOnTop",           // 9
	"VirtualDesktops",        // 10 
	"ShowMenue",              // 11
	"AutoRaiseTime",          // 12
	"InactiveWindowColor",    // 13
	"UrgentWindowColor",      // 14
	"StartClientsUrgent",     // 15
	"RootBackgroundColor",    // 16 
	"RootBackgroundPicture",  // 17
	"PagerActiveColor",       // 18
	"PagerVisibleColor",      // 19
	"PagerInvisibleColor",    // 20
	"ShowClientMachines",     // 21
	"Exec",                   // 22
	"ShowWinlist",            // 23
	"Style",                  // 24
	"Maximize1",              // 25
	"Maximize2",              // 26
	"TileSplit",              // 27
	"TileMaxWithTab",         // 28
	"TileStart",              // 29
	"TileMinframe",           // 30
	NULL };
	
	QString fname;
	QString p1,p2;
	QFileInfo acc;
	QStyle *qs;
	
	fname = qapp::get_cfile("defaults");
	QFile istr(fname);

	if(fname.isNull() || ! istr.open(QIODevice::ReadOnly))
	{
		if(! fname.isNull())
			perror("cannot open defaults file");
	}	
	else
	{
		while(! istr.atEnd())
		{
			QTextStream tis(istr.readLine(1024));
			tis >> p1;
			if(p1.isEmpty() || p1[0] == '#')
				continue;

			tis >> p2;
			if(p2.isEmpty())
				continue;

			i = 0;
			do
			{
				if(p1 == par[i])
					break;
			}
			while(par[++i] != NULL);
			
			switch(i)
			{
				case 0:
					ival = p2.toInt();
	
					if(ival < 10 || ival > 500)
						logmsg << "ToolbarHeight: Value out of range\n";
					else
						tb_height = ival;
						
					break;
	
				case 1:
					ival = p2.toInt();
		
					if(ival < 6 || ival > 500)
						logmsg << "WindowButtonSize: Value out of range\n";
					else
						windowbuttonsize = ival;
		
					break;
	
				case 2:
					ival = p2.toInt();
	
					if(ival < 1 || ival > 500)
						logmsg << "LowerBorderHeight: Value out of range\n";
					else
							lowerborderheight = ival;
	
					break;
	
				case 3:
					ival = p2.toInt();
	
					if(ival < 1 || ival > 500)
						logmsg << "LowerBorderWidth: Value out of range\n";
					else
						lowerborderwidth = ival;
	
					break;
	
				case 4:
					ival = p2.toInt();
	
					if(ival < 4 && ival > 500)
						logmsg << "PagerHeight: Value out of range\n";
					else
						pager_height = ival;
	
					break;
	
				case 5:
					if(p2.toLower() == "fixed")
						break;
						
					borderfont.setFamily(p2);
					break;
	
				case 6:
					ival = p2.toInt();
	
					if(ival < 4 || ival > 500)
						logmsg << "WindowFontHeight: Value out of range\n";
					else
						borderfont.setPixelSize(ival);
	
					break;
	
				case 7:
					if(p2.toLower() == "fixed")
						break;
						
					toolbarfont.setFamily(p2);
					break;
	
				case 8:
					ival = p2.toInt();
	
					if(ival < 4 || ival > 500)
						logmsg << "IconFontHeight: Value out of range\n";
					else
						toolbarfont.setPixelSize(ival);
	
					break;
	
				case 9:
					if(p2.toUpper() == "TRUE")
						toolbar_top = TRUE;
		
					break;
	
				case 10:
					ival = p2.toInt();
	
					if(ival < 1 || ival > MAXDESKS)
					{
						logmsg << "VirtualDesktops: Value out of range\n";
						vdesks = 3;
					}
					else
						vdesks = ival;
	
					break;	
	
				case 11:
					if(p2.toUpper() == "FALSE")
						show_menu = FALSE;
	
					break;
	
				case 12:
					ival = p2.toInt();
	
					if(ival < 0 || ival > 100000)
						logmsg << "AutoRaiseTime: Value out of range\n";
					else
						autofocustime = ival;
	
					break;
					
				case 13:
					inactive_bg = new QColor();
					inactive_bg->setNamedColor(p2);
					break;
	
				case 14:
					urgent_bg.setNamedColor(p2);
					break;
	
				case 15:
					if(p2.toUpper() == "FALSE")
						starturgent = FALSE;
	
					break;
	
				case 16:
					root_bg.setNamedColor(p2);
					break;
	
				case 17:
					acc.setFile(p2);
					if(! acc.isReadable())
						break;
						
					root_pix = p2;
					break;
	
				case 18:
					pager_active.setNamedColor(p2);
					break;
	
				case 19:
					pager_visible.setNamedColor(p2);
					break;
	
				case 20:
					pager_window.setNamedColor(p2);
					break;
	
				case 21:
					if(p2.toUpper() == "TRUE")
						showclientmachines = TRUE;

					break;

				case 22:
					if(start_restart == FALSE)
					{
						QString cml = p2+tis.readLine();
						initexec.push(new QByteArray(cml.toAscii()));
					}	
					break;
				
				case 23:
					if(p2.toUpper() == "FALSE")
						show_winlist = FALSE;
	
					break;

				case 24:
					if((qs = QStyleFactory::create(p2)) == NULL)
					{
						logmsg << "Unknown style: " << p2 << '\n';
						if(p2 == "Platinum")
							p2 = "Plastique";
							
						if((qs = QStyleFactory::create(p2)) == NULL)
							break;
					}
					QApplication::setStyle(qs);
					break;

				case 25:
				case 26:
					i2 = 0;
					while(1)
					{
						iar[i2] = p2.toInt(&ok);

						if(ok == FALSE || i2 > 2)
							break;

						tis >> p2;

						if(p2.isEmpty())
							break;

						i2++;
					}
					if(i2 < 3)
					{
						logmsg << "invalid parameter for maximize\n";
						break;
					}
					
					if(i == 25)
					{
						tmx1 = iar[0]; tmy1 = iar[1]; tmx2 = iar[2]; tmy2 = iar[3];
					}	
					else	
					{
						smx1 = iar[0]; smy1 = iar[1]; smx2 = iar[2]; smy2 = iar[3];
					}	

					break;	

				case 27:
					fval = p2.toFloat();

					if(fval < 1 || fval > 99)
						logmsg << "TileSplit: Value out of range\n";
					else
						tleftspace = fval/100;

					break;

				case 28: 
					ival = p2.toInt();

					if(ival < 0 || ival > 10000)
						logmsg << "TileMaxOnTab: Value out of range\n";
					else
						maxontab = ival;

					break;

				case 29:
					for(i2=0; i2 < 10; i2++)
					{
						ival = p2.toInt(&ok);

						if(ok == FALSE)
							break;

						if(ival < 1 || ival > 10)
						{
							logmsg << "TileStart: value out of range\n";
							continue;
						}

						sttiled[ival-1] = TRUE;

						tis >> p2;

						if(p2.isEmpty())
							break;
					}
					
				case 30:
					ival = p2.toInt();

					if(ival < 0 || ival > 10000)
						logmsg << "TileMinframe: Value out of range\n";
					else
						wminframe = ival;

					break;

				default:
					logmsg << "WM: unknown parameter: " << p1 << '\n';
			}
		}
		istr.close();
	}	
	
	QFontInfo info(borderfont);
	if(info.family() != borderfont.family())
		logmsg << "WM: no match for font " << borderfont.family() << ", using " << info.family() << " instead\n";

	QFontInfo tinfo(toolbarfont);
	if(tinfo.family() != toolbarfont.family())
		logmsg << "WM: no match for font " << toolbarfont.family() << ", using " << tinfo.family() << " instead\n";
	
	tc_height = tb_height-4;

	if(pager_height > tb_height)
		pager_height = tb_height;
	
	if(borderfont.pixelSize() > windowbuttonsize-3)
	{
		windowbuttonsize = borderfont.pixelSize()+3;
		logmsg << "WM: windowborder too small for font, set to " << windowbuttonsize << '\n';
	}

	if(toolbarfont.pixelSize() > tc_height-4)
	{
		tc_height = toolbarfont.pixelSize()+4;
		tb_height = tc_height+4;
		logmsg << "WM: toolbar contents too small for font, set to " << tc_height << '\n';
	}
}
