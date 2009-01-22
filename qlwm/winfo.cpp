/* winfo.cpp */

#include "defs.h"
#include "qapp.h"
#include "xwindow.h"
#include "defaults.h"
//#include "moc_winfo.cpp"

static int qcf[] = { qapp::Sticky,qapp::WindowListSkip,qapp::SmallFrame,qapp::NoResize,
qapp::NoTile,qapp::NoKey,qapp::NoScreen,qapp::StayOnBottom,qapp::ClassOnly };

winfo::winfo(QWidget *parent) : QWidget(parent)
{
	client = NULL;
	XSetWindowBorderWidth(QX11Info::display(), winId(), 1);
	setFont(defaults::borderfont);

	left = new QLabel(this);
	Q_CHECK_PTR(left);
	right = new QLabel(this);
	Q_CHECK_PTR(right);

	int i;
	QVBoxLayout *vl = new QVBoxLayout(this);
	vl->setMargin(2);
	
	static const char *qcs[] = { "Sticky","Skip windowlist","Small frame","No resize","No tiling",
	"Skip keyboard access","Skip in screenmode","Stay on bottom","Apply to all in class" };

	for(i=0; i < NITEMS; i++)
	{
		qcbox[i] = new QCheckBox(qcs[i], this);
		vl->addWidget(qcbox[i]);
	}

	left->setText(QString(" Title\n Name\n Class\n Location\n Invocation\n Geometry\n"));

	QHBoxLayout *hl1 = new QHBoxLayout();
	hl1->addWidget(left);
	hl1->addWidget(right);
	hl1->addStretch();
	vl->addSpacing(4);
	QHBoxLayout *hl2 = new QHBoxLayout();
	qb1 = new QPushButton("Permanent", this);
	qb2 = new QPushButton("Temporary", this);
	qb3 = new QPushButton(" Cancel  ", this);
	hl2->addWidget(qb1);
	hl2->addStretch();
	hl2->addWidget(qb2);
	hl2->addStretch();
	hl2->addWidget(qb3);

	setLayout(vl);
	vl->addLayout(hl1);
	vl->addLayout(hl2);
	
	connect(qb1, SIGNAL(pressed()), this, SLOT(release_permanent()));
	connect(qb2, SIGNAL(pressed()), this, SLOT(release_temporary()));
	connect(qb3, SIGNAL(pressed()), this, SLOT(release_cancel()));
}

void winfo::show_info(xwindow *cl, QString &wmname, QString &clientfqdn, QString &command,
QString &rn, QString &rc, int inc_w, int inc_h, int base_w, int base_h)
{
	QWidget *dt = QApplication::desktop();
	int i;
	
	if(client != NULL)
	{
		release_cancel();
		return;
	}	
		
	qapp::menu_open = TRUE;
	client = cl;
	res_name = rn;
	res_class = rc;
	
	int cx,cw,ch;
	cx = client->x()+(qapp::adesk*dt->width());
	cw = (inc_w > 1)?((client->width()-base_w)/inc_w):(client->width());
	ch = (inc_h > 1)?((client->height()-base_h)/inc_h):(client->height());

	QString gm;
	gm.sprintf("%dx%d+%d+%d", cw, ch, cx, client->y());

	QString txtr;
	QTextStream tsr(&txtr);

	tsr << ": " << wmname.left(50)     << " \n"
	    << ": " << res_name.left(50)   << " \n"
	    << ": " << res_class.left(50)  << " \n"
	    << ": " << clientfqdn.left(50) << " \n"
	    << ": " << command.left(50)    << " \n"
	    << ": " << gm << " \n";

	right->setText(txtr);

	for(i=0; i < NITEMS; i++)
		qcbox[i]->setChecked((client->get_pflags() & qcf[i])?TRUE:FALSE);

	XRaiseWindow(QX11Info::display(), winId());
	show();
	move((dt->width()/2)-(width()/2), (dt->height()/2)-(height()/2));
}

void winfo::release_permanent(void)
{
	QString fname,cclname;
	QByteArray sdat;
	
	if(client == NULL)
		return;

	if(qcbox[NITEMS-1]->isChecked())  // class only 
	{
		cclname = client->get_clientname();
	}
	else
	{
		cclname = res_name;
		cclname += ',';
		cclname += res_class;
	}
	
	if(defaults::cfdir.isNull())
	{
		release_temporary();
		return;
	}
	
	fname = defaults::cfdir;
	fname += "/appdefaults";

	QFile apfile(fname);
	QList<QString> lns;
	
	if(apfile.open(QIODevice::ReadOnly))
	{
		QTextStream lin(&apfile);
		while(! lin.atEnd())
		{
			QString cln = lin.readLine(1024);

			if(cln.indexOf(cclname) != 0)
				lns.append(cln);
		}
		apfile.close();
	}

	if(! apfile.open(QIODevice::WriteOnly))
	{
		sdat = fname.toAscii();
		perror(sdat.data());
		release_temporary();
		return;
	}
	
	for(int i=0; i < lns.size(); i++)
	{
		sdat = lns.at(i).toAscii();
		sdat += '\n';
		apfile.write(sdat.data(), sdat.length());
	}	
	
	int i;
	bool ref=FALSE;
	static const char *qcs[] = { "Sticky","WindowListSkip","SmallFrame","NoResize","NoTile","NoKey","NoScreen","StayOnBottom" };
	QString ostr;
	
	for(i=0; i < NITEMS-1; i++)
	{
		if(qcbox[i]->isChecked())
		{
			if(! ref)
			{
				ostr = cclname;
				ostr += ' ';
				ref = TRUE;
			}
			else
				ostr = ',';

			ostr += qcs[i];
			sdat = ostr.toAscii();
			apfile.write(sdat.data(), sdat.length());
		}
	}
	if(ref)
		apfile.write("\n", 1);

	apfile.close();	

	int tl = (client->get_pflags() & (qapp::NoTile|qapp::Sticky));
	qapp::read_cprops();

	if(qapp::is_tileddesk())
	{
		if(tl && ! (client->get_pflags() & (qapp::NoTile|qapp::Sticky)))
		{
			qapp::tile_order(client);
		}
		else if(! tl && (client->get_pflags() & (qapp::NoTile|qapp::Sticky)))
		{
			qapp::tile_order(qapp::tmaxclient);
			client->raise();
		}
	}	
	release_cancel();
}

void winfo::release_temporary(void)
{
	if(client == NULL)
		return;
		
	int flags=0,i;
	
	for(i=0; i < NITEMS; i++)
	{
		if(qcbox[i]->isChecked())
			flags |= qcf[i];
	}

	int tl = (client->get_pflags() & (qapp::NoTile|qapp::Sticky));
	client->set_pflags(flags);

	if(qapp::is_tileddesk())
	{
		if(tl && ! (client->get_pflags() & (qapp::NoTile|qapp::Sticky)))
		{
			qapp::tile_order(client);
		}
		else if(! tl && (client->get_pflags() & (qapp::NoTile|qapp::Sticky)))
		{
			qapp::tile_order(qapp::tmaxclient);
			client->raise();
		}
	}	
	release_cancel();
}

void winfo::release_cancel(void)
{
	if(client == NULL)
		return;

	hide();
	XSync(QX11Info::display(), FALSE);
	qb1->setDown(FALSE);
	qb2->setDown(FALSE);
	qb3->setDown(FALSE);
	client = NULL;
	qapp::menu_open = FALSE;
}
QWidget *winfo::get_client(void)
{
    return(client);
}
