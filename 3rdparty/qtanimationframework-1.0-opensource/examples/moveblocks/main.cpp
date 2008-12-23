/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QtGui>
#include <time.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include <common.h>
# include <qstate.h>
# include <qstategroup.h>
# include <qtransition.h>
# include <qanimation.h>
#endif

class StateSwitcher : public QObject
{
    Q_OBJECT
public:
    StateSwitcher()
        : lastIndex(0)
    { }

    QList<QtState *> states;
public slots:
    void switchState()
    {
        int n;
        while ((n = (qrand() % states.size())) == lastIndex)
        { }
        lastIndex = n;
        states.at(n)->activate();
    }

private:
    int lastIndex;
};

class QGraphicsRectWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    QGraphicsRectWidget()
        : QGraphicsWidget()
    { }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
               QWidget *)
    {
        painter->fillRect(rect(), Qt::blue);
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if 0
    QWidget window;
    QPalette palette;
    palette.setBrush(QPalette::Window, Qt::black);
    window.setPalette(palette);
    QPushButton *button1 = new QPushButton("A", &window);
    QPushButton *button2 = new QPushButton("B", &window);
    QPushButton *button3 = new QPushButton("C", &window);
    QPushButton *button4 = new QPushButton("D", &window);
#else
    QGraphicsRectWidget *button1 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button2 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button3 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button4 = new QGraphicsRectWidget;
    button2->setZValue(1);
    button3->setZValue(2);
    button4->setZValue(3);
    QGraphicsScene scene(0, 0, 300, 300);
    scene.setBackgroundBrush(Qt::black);
    scene.addItem(button1);
    scene.addItem(button2);
    scene.addItem(button3);
    scene.addItem(button4);

    QGraphicsView window(&scene);
    window.setFrameStyle(0);
    window.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    window.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    window.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif

    QtStateGroup group;
    QtState *state1 = new QtState(&group);
    QtState *state2 = new QtState(&group);
    QtState *state3 = new QtState(&group);
    QtState *state4 = new QtState(&group);
    QtState *state5 = new QtState(&group);
    QtState *state6 = new QtState(&group);
    QtState *state7 = new QtState(&group);

    state1->setGeometry(button1, QRect(100, 0, 50, 50));
    state1->setGeometry(button2, QRect(150, 0, 50, 50));
    state1->setGeometry(button3, QRect(200, 0, 50, 50));
    state1->setGeometry(button4, QRect(250, 0, 50, 50));
    state2->setGeometry(button1, QRect(250, 100, 50, 50));
    state2->setGeometry(button2, QRect(250, 150, 50, 50));
    state2->setGeometry(button3, QRect(250, 200, 50, 50));
    state2->setGeometry(button4, QRect(250, 250, 50, 50));
    state3->setGeometry(button1, QRect(150, 250, 50, 50));
    state3->setGeometry(button2, QRect(100, 250, 50, 50));
    state3->setGeometry(button3, QRect(50, 250, 50, 50));
    state3->setGeometry(button4, QRect(0, 250, 50, 50));
    state4->setGeometry(button1, QRect(0, 150, 50, 50));
    state4->setGeometry(button2, QRect(0, 100, 50, 50));
    state4->setGeometry(button3, QRect(0, 50, 50, 50));
    state4->setGeometry(button4, QRect(0, 0, 50, 50));
    state5->setGeometry(button1, QRect(100, 100, 50, 50));
    state5->setGeometry(button2, QRect(150, 100, 50, 50));
    state5->setGeometry(button3, QRect(100, 150, 50, 50));
    state5->setGeometry(button4, QRect(150, 150, 50, 50));
    state6->setGeometry(button1, QRect(50, 50, 50, 50));
    state6->setGeometry(button2, QRect(200, 50, 50, 50));
    state6->setGeometry(button3, QRect(50, 200, 50, 50));
    state6->setGeometry(button4, QRect(200, 200, 50, 50));
    state7->setGeometry(button1, QRect(0, 0, 50, 50));
    state7->setGeometry(button2, QRect(250, 0, 50, 50));
    state7->setGeometry(button3, QRect(0, 250, 50, 50));
    state7->setGeometry(button4, QRect(250, 250, 50, 50));
    state1->activate();

    QtTransition transition(0, 0);
    QtAnimation *anim;
    anim = new QtAnimation(button4, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QtEasingCurve::OutElastic);
    transition.add(anim);
    transition.beginSequentialGroup();
    transition.addPause(100);
    anim = new QtAnimation(button3, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QtEasingCurve::OutElastic);
    transition.add(anim);
    transition.endGroup();
    transition.beginSequentialGroup();
    transition.addPause(150);
    anim = new QtAnimation(button2, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QtEasingCurve::OutElastic);
    transition.add(anim);
    transition.endGroup();
    transition.beginSequentialGroup();
    transition.addPause(200);
    anim = new QtAnimation(button1, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QtEasingCurve::OutElastic);
    transition.add(anim);
    transition.endGroup();

    StateSwitcher switcher;
    switcher.states << state1 << state2 << state3
                    << state4 << state5 << state6
                    << state7;

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &switcher, SLOT(switchState()));
    timer.start(1250);

    window.resize(300, 300);
    window.show();

    qsrand(time(0));

    return app.exec();
}

#include "main.moc"
