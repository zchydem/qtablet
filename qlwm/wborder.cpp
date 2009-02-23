/*
*  File      : wborder.cpp
*  Written by: alinden@gmx.de
*  Copyright : GPL
*
*  Creates the window borders
*/

#include "defs.h"
#include "defaults.h"
#include "qapp.h"
//#include "moc_wborder.cpp"

wframe::wframe(QWidget *parent) : QLabel(parent)
{
	setFrameStyle(QFrame::Panel|QFrame::Raised);
	setLineWidth(1);
	setMinimumWidth(1);
	setWordWrap(FALSE);
}

void wframe::set_text(QString btxt, bool center)
{
	setAlignment(center?Qt::AlignCenter:Qt::AlignLeft|Qt::AlignVCenter);
	setText(btxt);
}

void wframe::resizeEvent(QResizeEvent *)
{
	repaint();
}

void wframe::mousePressEvent(QMouseEvent *event)
{
	emit press(event);
	emit press();

	if(event->button() == Qt::RightButton)
	{
		emit right_press();
	}	
	else if(event->button() == Qt::LeftButton)
	{
		emit left_press(event);
		emit left_press();
	}
	else if(event->button() == Qt::MidButton)
	{
		emit mid_press();
	}
}

void wframe::mouseReleaseEvent(QMouseEvent *event)
{
	emit release(event);

	if(event->button() == Qt::RightButton)
	{
		emit right_release();
	}
	else if(event->button() == Qt::LeftButton)
	{
		emit left_release(event);
	}
	else if(event->button() == Qt::MidButton)
	{
		emit mid_release();
	}
}

void wframe::mouseMoveEvent(QMouseEvent *event)
{
	emit mouse_move(event);
}

lborder::lborder(QWidget *parent) : QWidget(parent)
{
	layout = new QHBoxLayout(this);
	Q_CHECK_PTR(layout);
	leftframe = new wframe(this);
	Q_CHECK_PTR(leftframe);
	midframe = new wframe(this);
	Q_CHECK_PTR(midframe);
	rightframe = new wframe(this);
	Q_CHECK_PTR(rightframe);
	
	leftframe->setFixedWidth(defaults::lowerborderwidth);
	rightframe->setFixedWidth(defaults::lowerborderwidth);
	setFixedHeight(defaults::lowerborderheight);

	leftframe->setCursor(QCursor(Qt::SizeBDiagCursor));
	midframe->setCursor(QCursor(Qt::SizeVerCursor));
	rightframe->setCursor(QCursor(Qt::SizeFDiagCursor));

	layout->addWidget(leftframe);
	layout->addWidget(midframe);
	layout->addWidget(rightframe);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);
}

void uborder::set_small(void)
{
    set_normal();
    return;
	setFixedHeight(defaults::lowerborderheight);
	leftframe->setFixedWidth(defaults::lowerborderwidth);
	rightframe->setFixedWidth(defaults::lowerborderwidth);
	leftframe->setPixmap(QPixmap());
	rightframe->setPixmap(QPixmap());
	midframe->setText("");
}

void uborder::set_normal(void)
{
        setFixedHeight( 90 /*defaults::windowbuttonsize */);
        leftframe->setFixedWidth( 90/*defaults::windowbuttonsize */);
        rightframe->setFixedWidth(90/*defaults::windowbuttonsize*/);
	leftframe->setPixmap(*qapp::leftwinpix);
	rightframe->setPixmap(*qapp::rightwinpix);
}

uborder::uborder(bool showleft, QWidget *parent) : QWidget(parent)
{
	layout = new QHBoxLayout(this);
	Q_CHECK_PTR(layout);
        //setFixedHeight(defaults::windowbuttonsize);
        setFixedHeight( 90 );
        setWindowOpacity(0.5);

	if(showleft)
	{
		leftframe = new wframe(this);
		Q_CHECK_PTR(leftframe);
                leftframe->setFixedWidth( 90 );
		leftframe->setAlignment(Qt::AlignCenter);
		leftframe->setPixmap(*qapp::leftwinpix);
                leftframe->setFixedHeight( 90 );
		layout->addWidget(leftframe);
	}
	
	midframe = new wframe(this);
        midframe->setFixedHeight( 90 );
	Q_CHECK_PTR(midframe);
	rightframe = new wframe(this);
	Q_CHECK_PTR(rightframe);
        rightframe->setFixedWidth( 90 );
        rightframe->setFixedHeight( 90 );
	rightframe->setAlignment(Qt::AlignCenter);
	rightframe->setPixmap(*qapp::rightwinpix);

	layout->addWidget(midframe);
	layout->addWidget(rightframe);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);
}
