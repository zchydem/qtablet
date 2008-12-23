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

#include "boat.h"
#include "bomb.h"
#include "pixmapitem.h"
#include "graphicsscene.h"

static const int MAX_BOMB = 5;

Boat::Boat(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent,wFlags), currentSpeed(0), currentDirection(Boat::None), movementAnimation(0),isAboutToDie(false)
{
    pixmapItem = new PixmapItem(QString("boat"),GraphicsScene::Big, this);
    setZValue(4);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFlags(QGraphicsItem::ItemIsMovable |  QGraphicsItem::ItemIsFocusable);
    resize(pixmapItem->boundingRect().size());
}

void Boat::keyPressEvent(QKeyEvent * event)
{
    if (isAboutToDie)
        return;
    switch(event->key()) {
      case Qt::Key_Left :
          if (currentDirection == Boat::None && currentSpeed == 0)
              currentDirection = Boat::Left;
              if (currentDirection == Boat::Left && currentSpeed < 3) {
                      currentSpeed++;
                      updateBoatMovement();
              }
              else if (currentDirection == Boat::Right && currentSpeed != 0) {
                      currentSpeed--;
                      updateBoatMovement();
              }
              if (currentSpeed == 0)
                      currentDirection = Boat::None;
              break;

      case Qt::Key_Right :
            if (currentDirection == Boat::None && currentSpeed == 0)
                currentDirection = Boat::Right;
            if (currentDirection == Boat::Right && currentSpeed < 3) {
                currentSpeed++;
                updateBoatMovement();
            }
            else if (currentDirection == Boat::Left && currentSpeed != 0) {
                currentSpeed--;
                updateBoatMovement();
            }
            if (currentSpeed == 0)
                currentDirection = Boat::None;
            break;

      case Qt::Key_Down :
            if (bombs.size() <= MAX_BOMB && !isAboutToDie) {
                Bomb *b = new Bomb();
                b->setPos(x() - b->size().width(), y());
                scene()->addItem(b);
                b->launch(Bomb::Left);
                bombs.insert(b);
                connect(b, SIGNAL(destroyed(QObject *)),this,SLOT(onBombDestroyed(QObject *)));
            }
            QGraphicsWidget::keyPressEvent(event);
            break;

      case Qt::Key_Up :
            if (bombs.size() <= MAX_BOMB && !isAboutToDie) {
                Bomb *b = new Bomb();
                b->setPos(x()+size().width(), y());
                scene()->addItem(b);
                b->launch(Bomb::Right);
                bombs.insert(b);
                connect(b, SIGNAL(destroyed(QObject *)),this,SLOT(onBombDestroyed(QObject *)));
            }
            QGraphicsWidget::keyPressEvent(event);
            break;

      default : QGraphicsWidget::keyPressEvent(event);break;
    }
    event->accept();
}

void Boat::updateBoatMovement()
{
    if (isAboutToDie)
        return;
    if (movementAnimation) {
        movementAnimation->stop();
        delete movementAnimation;
        movementAnimation = 0;
    }
    if (currentDirection == Boat::Left && currentSpeed != 0) {
        movementAnimation = QtAnimation::posAnimation(this,QPointF(0,y()));
        movementAnimation->setDuration(x()/currentSpeed*15);
        movementAnimation->start();
        connect(movementAnimation,SIGNAL(finished()),this,SLOT(onAnimationMovementFinished()));
    }
    else if (currentDirection == Boat::Right && currentSpeed != 0) {
        movementAnimation = QtAnimation::posAnimation(this,QPointF(scene()->width()-size().width(),y()));
        movementAnimation->setDuration((scene()->width()-size().width()-x())/currentSpeed*15);
        movementAnimation->start();
        connect(movementAnimation,SIGNAL(finished()),this,SLOT(onAnimationMovementFinished()));
    }
}

void Boat::onAnimationMovementFinished()
{
     currentDirection = Boat::None;
     currentSpeed = 0;
}

void Boat::destroy()
{
    isAboutToDie = true;
    if (movementAnimation) {
        movementAnimation->stop();
        movementAnimation->deleteLater();
        movementAnimation = 0;
    }
    foreach (Bomb *bomb,bombs) {
        bomb->destroy();
    }
    deleteLater();

}

void Boat::onBombDestroyed(QObject * obj)
{
    bombs.remove(static_cast<Bomb *>(obj));
}

