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
#if defined(QT_EXPERIMENTAL_SOLUTION)
# include <common.h>
# include <qstate.h>
# include <qstategroup.h>
# include <qtransition.h>
# include <qanimationgroup.h>
# include <qanimation.h>
#endif

class Pixmap : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(double rotationZ READ rotationZ WRITE setRotationZ)
public:
    Pixmap(const QPixmap &pix, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), p(pix), currentRotation(0.0)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

    double rotationZ() const
    {
        return currentRotation;
    }

    void setRotationZ(double rr)
    {
        currentRotation = rr;
        QPointF c = boundingRect().center();
        setTransform(QTransform().translate(c.x(), c.y()).rotate(rr, Qt::ZAxis).translate(-c.x(), -c.y()));
    }

protected:
    QSizeF sizeHint(Qt::SizeHint, const QSizeF & = QSizeF())
    {
        return QSizeF(p.width(), p.height());
    }

private:
    QPixmap p;
    double currentRotation;
};

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(states);

    QApplication app(argc, argv);

    // Text edit and button
    QTextEdit *edit = new QTextEdit;
    edit->setText("asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy!");

    QPushButton *button = new QPushButton;
    QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget;
    buttonProxy->setWidget(button);
    QGraphicsProxyWidget *editProxy = new QGraphicsProxyWidget;
    editProxy->setWidget(edit);

    QGroupBox *box = new QGroupBox;
    box->setFlat(true);
    box->setTitle("Options");

    QVBoxLayout *layout2 = new QVBoxLayout;
    box->setLayout(layout2);
    layout2->addWidget(new QRadioButton("Herring"));
    layout2->addWidget(new QRadioButton("Blue Parrot"));
    layout2->addWidget(new QRadioButton("Petunias"));
    layout2->addStretch();

    QGraphicsProxyWidget *boxProxy = new QGraphicsProxyWidget;
    boxProxy->setWidget(box);

    // Parent widget
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, widget);
    layout->addItem(editProxy);
    layout->addItem(buttonProxy);
    widget->setLayout(layout);

    Pixmap *p1 = new Pixmap(QPixmap(":/digikam.png"));
    Pixmap *p2 = new Pixmap(QPixmap(":/akregator.png"));
    Pixmap *p3 = new Pixmap(QPixmap(":/accessories-dictionary.png"));
    Pixmap *p4 = new Pixmap(QPixmap(":/k3b.png"));
    Pixmap *p5 = new Pixmap(QPixmap(":/help-browser.png"));
    Pixmap *p6 = new Pixmap(QPixmap(":/kchart.png"));

    QGraphicsScene scene(0, 0, 400, 300);
    scene.setBackgroundBrush(scene.palette().window());
    scene.addItem(widget);
    scene.addItem(boxProxy);
    scene.addItem(p1);
    scene.addItem(p2);
    scene.addItem(p3);
    scene.addItem(p4);
    scene.addItem(p5);
    scene.addItem(p6);

    QtState *state1 = new QtState(&scene);
    QtState *state2 = new QtState(&scene);
    QtState *state3 = new QtState(&scene);
    QtStateGroup group;
    group.addState(state1);
    group.addState(state2);
    group.addState(state3);

    // State 1
    state1->setProperty(button, "text", "Switch to state 2");
    state1->setConnectionState(button, SIGNAL(clicked()), state2, SLOT(activate()));
    state1->setGeometry(widget, QRectF(0, 0, 400, 150));
    state1->setGeometry(box, QRect(-200, 150, 200, 150));
    state1->setGeometry(p1, QRectF(68, 185, 64, 64));
    state1->setGeometry(p2, QRectF(168, 185, 64, 64));
    state1->setGeometry(p3, QRectF(268, 185, 64, 64));
    state1->setGeometry(p4, QRectF(68-150, 48-150, 64, 64));
    state1->setGeometry(p5, QRectF(168, 48-150, 64, 64));
    state1->setGeometry(p6, QRectF(268+150, 48-150, 64, 64));
    state1->setProperty(p1, "rotationAngleZ", double(0));
    state1->setProperty(p2, "rotationAngleZ", double(0));
    state1->setProperty(p3, "rotationAngleZ", double(0));
    state1->setProperty(p4, "rotationAngleZ", double(-270));
    state1->setProperty(p5, "rotationAngleZ", double(-90));
    state1->setProperty(p6, "rotationAngleZ", double(270));
    state1->setOpacity(boxProxy, double(0.0));
    state1->setOpacity(p1, double(1));
    state1->setOpacity(p2, double(1));
    state1->setOpacity(p3, double(1));
    state1->setOpacity(p4, double(0));
    state1->setOpacity(p5, double(0));
    state1->setOpacity(p6, double(0));

    // State 2
    state2->setProperty(button, "text", "Switch back state 3");
    state2->setConnectionState(button, SIGNAL(clicked()), state3, SLOT(activate()));
    state2->setGeometry(widget, QRectF(200, 150, 200, 150));
    state2->setGeometry(box, QRect(9, 150, 190, 150));
    state2->setGeometry(p1, QRectF(68-150, 185+150, 64, 64));
    state2->setGeometry(p2, QRectF(168, 185+150, 64, 64));
    state2->setGeometry(p3, QRectF(268+150, 185+150, 64, 64));
    state2->setGeometry(p4, QRectF(64, 48, 64, 64));
    state2->setGeometry(p5, QRectF(168, 48, 64, 64));
    state2->setGeometry(p6, QRectF(268, 48, 64, 64));
    state2->setProperty(p1, "rotationAngleZ", double(-270));
    state2->setProperty(p2, "rotationAngleZ", double(90));
    state2->setProperty(p3, "rotationAngleZ", double(270));
    state2->setProperty(p4, "rotationAngleZ", double(0));
    state2->setProperty(p5, "rotationAngleZ", double(0));
    state2->setProperty(p6, "rotationAngleZ", double(0));
    state2->setOpacity(boxProxy, double(1.0));
    state2->setOpacity(p1, double(0));
    state2->setOpacity(p2, double(0));
    state2->setOpacity(p3, double(0));
    state2->setOpacity(p4, double(1));
    state2->setOpacity(p5, double(1));
    state2->setOpacity(p6, double(1));

    // State 3
    state3->setProperty(button, "text", "Switch back state 1");
    state3->setGeometry(p1, QRectF(5, 5, 64, 64));
    state3->setGeometry(p2, QRectF(5, 5 + 64 + 5, 64, 64));
    state3->setGeometry(p3, QRectF(5, 5 + (64 + 5) + 64, 64, 64));
    state3->setGeometry(p4, QRectF(5 + 64 + 5, 5, 64, 64));
    state3->setGeometry(p5, QRectF(5 + 64 + 5, 5 + 64 + 5, 64, 64));
    state3->setGeometry(p6, QRectF(5 + 64 + 5, 5 + (64 + 5) + 64, 64, 64));
    state3->setGeometry(widget, QRectF(138, 5, 400 - 138, 200));
    state3->setGeometry(box, QRect(5, 205, 400, 90));
    state3->setConnectionState(button, SIGNAL(clicked()), state1, SLOT(activate()));
    state3->setOpacity(p1, double(1));
    state3->setOpacity(p2, double(1));
    state3->setOpacity(p3, double(1));
    state3->setOpacity(p4, double(1));
    state3->setOpacity(p5, double(1));
    state3->setOpacity(p6, double(1));

    QtTransition transition1(state1, state2);
    QtAnimationGroup *delayedBoxAnimation = new QtAnimationGroup(QtAnimationGroup::Sequential);
    delayedBoxAnimation->addPause(250);
    delayedBoxAnimation->add(new QtAnimation(box, "geometry"));
    transition1.add(delayedBoxAnimation);
    transition1.add(new QtAnimation(widget, "geometry"));
    transition1.add(new QtAnimation(p1, "geometry"));
    transition1.add(new QtAnimation(p2, "geometry"));
    transition1.add(new QtAnimation(p3, "geometry"));
    transition1.add(new QtAnimation(p4, "geometry"));
    transition1.add(new QtAnimation(p5, "geometry"));
    transition1.add(new QtAnimation(p6, "geometry"));
    transition1.add(new QtAnimation(p1, "rotationAngleZ"));
    transition1.add(new QtAnimation(p2, "rotationAngleZ"));
    transition1.add(new QtAnimation(p3, "rotationAngleZ"));
    transition1.add(new QtAnimation(p4, "rotationAngleZ"));
    transition1.add(new QtAnimation(p5, "rotationAngleZ"));
    transition1.add(new QtAnimation(p6, "rotationAngleZ"));
    transition1.add(new QtAnimation(p1, "opacity"));
    transition1.add(new QtAnimation(p2, "opacity"));
    transition1.add(new QtAnimation(p3, "opacity"));
    transition1.add(new QtAnimation(p4, "opacity"));
    transition1.add(new QtAnimation(p5, "opacity"));
    transition1.add(new QtAnimation(p6, "opacity"));

    QtTransition transition2(state2, state3);
    transition2.add(new QtAnimation(box, "geometry"));
    transition2.add(new QtAnimation(widget, "geometry"));
    transition2.add(new QtAnimation(p1, "geometry"));
    transition2.add(new QtAnimation(p2, "geometry"));
    transition2.add(new QtAnimation(p3, "geometry"));
    transition2.add(new QtAnimation(p4, "geometry"));
    transition2.add(new QtAnimation(p5, "geometry"));
    transition2.add(new QtAnimation(p6, "geometry"));
    transition2.add(new QtAnimation(p1, "rotationAngleZ"));
    transition2.add(new QtAnimation(p2, "rotationAngleZ"));
    transition2.add(new QtAnimation(p3, "rotationAngleZ"));
    transition2.add(new QtAnimation(p4, "rotationAngleZ"));
    transition2.add(new QtAnimation(p5, "rotationAngleZ"));
    transition2.add(new QtAnimation(p6, "rotationAngleZ"));
    transition2.add(new QtAnimation(p1, "opacity"));
    transition2.add(new QtAnimation(p2, "opacity"));
    transition2.add(new QtAnimation(p3, "opacity"));
    transition2.add(new QtAnimation(p4, "opacity"));
    transition2.add(new QtAnimation(p5, "opacity"));
    transition2.add(new QtAnimation(p6, "opacity"));

    QtTransition transition3(state3, state1);
    transition3.add(new QtAnimation(box, "geometry"));
    transition3.add(new QtAnimation(widget, "geometry"));
    transition3.add(new QtAnimation(p1, "geometry"));
    transition3.add(new QtAnimation(p2, "geometry"));
    transition3.add(new QtAnimation(p3, "geometry"));
    transition3.add(new QtAnimation(p4, "geometry"));
    transition3.add(new QtAnimation(p5, "geometry"));
    transition3.add(new QtAnimation(p6, "geometry"));
    transition3.add(new QtAnimation(p1, "rotationAngleZ"));
    transition3.add(new QtAnimation(p2, "rotationAngleZ"));
    transition3.add(new QtAnimation(p3, "rotationAngleZ"));
    transition3.add(new QtAnimation(p4, "rotationAngleZ"));
    transition3.add(new QtAnimation(p5, "rotationAngleZ"));
    transition3.add(new QtAnimation(p6, "rotationAngleZ"));
    transition3.add(new QtAnimation(p1, "opacity"));
    transition3.add(new QtAnimation(p2, "opacity"));
    transition3.add(new QtAnimation(p3, "opacity"));
    transition3.add(new QtAnimation(p4, "opacity"));
    transition3.add(new QtAnimation(p5, "opacity"));
    transition3.add(new QtAnimation(p6, "opacity"));

    state1->activate();

/*
  QtTransition state1_state2(state1, state2);
  state1_state2.setOrder(QtAnimationGroup::Sequential);
  //  state1_state2.add(anim2);
  state1_state2.add(new QtAnimation(box, "geometry"));
*/
/*
  QtTransition state2_state3(state2, state3);
  state2_state3.add(new QtAnimation(box, "geometry"));

  QtTransition state1_state3(state1, state3);
  state1_state3.setOrder(QtAnimationGroup::Sequential);
  state1_state3.add(new QtAnimation(box, "geometry"));
*/
    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}

#include "main.moc"

