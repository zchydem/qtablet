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

#include "torpedo.h"
#include "pixmapitem.h"
#include "boat.h"
#include "graphicsscene.h"

Torpedo::Torpedo(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent,wFlags), currentSpeed(0), launchAnimation(0)
{
    pixmapItem = new PixmapItem(QString("torpedo"),GraphicsScene::Big, this);
    setZValue(2);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFlags(QGraphicsItem::ItemIsMovable);
    resize(pixmapItem->boundingRect().size());
}

void Torpedo::launch()
{
    launchAnimation = QtAnimation::posAnimation(this,QPointF(x(),qobject_cast<GraphicsScene *>(scene())->sealLevel() - 15),QtEasingCurve::OutElastic);
    launchAnimation->setDuration(y()/currentSpeed*10);
    launchAnimation->start();
    connect(launchAnimation,SIGNAL(finished()),this,SLOT(onAnimationLaunchFinished()));
    connect(launchAnimation,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationLaunchValueChanged(const QVariant &)));
}

void Torpedo::setCurrentSpeed(int speed)
{
    if (speed < 0)
        return;
    currentSpeed = speed;
}

void Torpedo::onAnimationLaunchFinished()
{
    destroy();
}

void Torpedo::onAnimationLaunchValueChanged(const QVariant &)
{
    foreach (QGraphicsItem * item , collidingItems(Qt::IntersectsItemBoundingRect)) {
        if (Boat *b = dynamic_cast<Boat *>(item))
              b->destroy();
    }
}

void Torpedo::destroy()
{
    currentSpeed = 0;
    if (launchAnimation) {
        disconnect(launchAnimation,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationLaunchValueChanged(const QVariant &)));
        disconnect(launchAnimation,SIGNAL(finished()),this,SLOT(onAnimationLaunchFinished()));
        launchAnimation->stop();
        launchAnimation->deleteLater();
        launchAnimation = 0;
    }
    deleteLater();
}
