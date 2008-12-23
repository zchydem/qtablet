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

#include "submarine.h"
#include "torpedo.h"
#include "pixmapitem.h"
#include "graphicsscene.h"

SubMarine::SubMarine(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent,wFlags), currentSpeed(0), currentRotation(0.0), rotateLeft(false), currentDirection(SubMarine::None), movementAnimation(0), returnAnimation(0), isAboutToDie(false)
{
    pixmapItem = new PixmapItem(QString("submarine"),GraphicsScene::Big, this);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setZValue(5);
    setFlags(QGraphicsItem::ItemIsMovable);
    resize(pixmapItem->boundingRect().width(),pixmapItem->boundingRect().height());
}

SubMarine::~SubMarine()
{
}

void SubMarine::setCurrentDirection(SubMarine::Movement direction)
{
    if (direction == currentDirection)
        return;
    if (direction == SubMarine::Right && currentDirection == SubMarine::None) {
          setRotation(180.);
          rotateLeft = true;
    }
    currentDirection = direction;
    updateSubMarineMovement();
}

void SubMarine::setCurrentSpeed(int speed)
{
    if (speed < 0 || speed > 3)
        return;
    currentSpeed = speed;
    updateSubMarineMovement();
}

double SubMarine::rotation() const
{
    return currentRotation;
}

void SubMarine::setRotation(double rr)
{
    currentRotation = rr;
    QPointF c = boundingRect().center();
    setTransform(QTransform().translate(c.x(), c.y()).rotate(rr, Qt::YAxis).translate(-c.x(), -c.y()));
}

void SubMarine::updateSubMarineMovement()
{
     if (movementAnimation) {
        movementAnimation->stop();
        delete movementAnimation;
        movementAnimation = 0;
     }
     if (currentDirection == SubMarine::Left && currentSpeed != 0) {
        movementAnimation = QtAnimation::posAnimation(this,QPointF(0,y()));
        movementAnimation->setDuration(x()/currentSpeed*15);
        movementAnimation->start();
        connect(movementAnimation,SIGNAL(finished()),this,SLOT(onAnimationMovementFinished()));
        connect(movementAnimation,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationMovementValueChanged(const QVariant &)));
     }
     else if (currentDirection == SubMarine::Right && currentSpeed != 0) {
        movementAnimation = QtAnimation::posAnimation(this,QPointF(scene()->width()-size().width(),y()));
        movementAnimation->setDuration((scene()->width()-size().width()-x())/currentSpeed*15);
        movementAnimation->start();
        connect(movementAnimation,SIGNAL(finished()),this,SLOT(onAnimationMovementFinished()));
        connect(movementAnimation,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationMovementValueChanged(const QVariant &)));
     }
}

void SubMarine::onAnimationMovementFinished()
{
     returnAnimation = QtAnimation::propertyAnimation(this,"rotation",rotateLeft ? 360. : 180.,500);;
     rotateLeft = currentDirection == SubMarine::Left;
     returnAnimation->start();
     connect(returnAnimation,SIGNAL(finished()),this,SLOT(onAnimationRotationFinished()));
}

void SubMarine::onAnimationMovementValueChanged(const QVariant &)
{
    if (qrand() % 200 + 1 == 3)
        launchTorpedo(qrand() % 3 + 1);
}

void SubMarine::onAnimationRotationFinished()
{
    returnAnimation->deleteLater();
    returnAnimation = 0;
    currentDirection == SubMarine::Right ? currentDirection=SubMarine::Left : currentDirection=SubMarine::Right;
    updateSubMarineMovement();
}

void SubMarine::launchTorpedo(int speed)
{
    Torpedo * torp = new Torpedo();
    scene()->addItem(torp);
    torpedos.insert(torp);
    torp->setPos(x(),y());
    torp->setCurrentSpeed(speed);
    torp->launch();
    connect(torp,SIGNAL(destroyed(QObject *)),this,SLOT(onTorpedoDestroyed(QObject *)));
}

void SubMarine::explose()
{
    //Here a beautiful animation
    if (movementAnimation) {
        movementAnimation->stop();
        movementAnimation->deleteLater();
    }
    destroy();
}

void SubMarine::destroy()
{
    isAboutToDie = true;
    if (movementAnimation) {
        movementAnimation->stop();
        movementAnimation->deleteLater();
        movementAnimation = 0;
    }
    if (returnAnimation) {
        returnAnimation->stop();
        returnAnimation->deleteLater();
        returnAnimation = 0;
    }
    foreach (Torpedo *torp,torpedos) {
        torp->destroy();
    }
    deleteLater();
}

void SubMarine::onTorpedoDestroyed(QObject * obj)
{
     if (!isAboutToDie)
        torpedos.remove(static_cast<Torpedo *>(obj)); //Warning don't use the obj pointer here
}
