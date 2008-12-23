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

#include "bomb.h"
#include "submarine.h"
#include "pixmapitem.h"

Bomb::Bomb(QGraphicsItem * parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent,wFlags), launchAnimation(0)
{
    pixmapItem = new PixmapItem(QString("bomb"),GraphicsScene::Big, this);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFlags(QGraphicsItem::ItemIsMovable);
    setZValue(2);
    resize(pixmapItem->boundingRect().size());
}

void Bomb::launch(Bomb::Direction direction)
{
    launchAnimation = new QtAnimationGroup(QtAnimationGroup::Sequential);
    qreal delta = direction == Right ? 20 : - 20;
    launchAnimation->add(QtAnimation::posAnimation(this,QPointF(x() + delta,y() - 20),150));
    launchAnimation->add(QtAnimation::posAnimation(this,QPointF(x() + delta*2, y() ),150));
    QtAnimation * anim = QtAnimation::posAnimation(this,QPointF(x() + delta*2,scene()->height()),500);
    launchAnimation->add(anim);
    anim->setDuration(y()/2*60);
    launchAnimation->start();
    connect(launchAnimation,SIGNAL(finished()),this,SLOT(onAnimationLaunchFinished()));
    connect(anim,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationLaunchValueChanged(const QVariant &)));
}

void Bomb::onAnimationLaunchFinished()
{
     destroy();
}

void Bomb::onAnimationLaunchValueChanged(const QVariant &)
{
    if (collidingItems(Qt::IntersectsItemBoundingRect).size() > 1) {
        foreach (QGraphicsItem * item , collidingItems(Qt::IntersectsItemBoundingRect)) {
          if (SubMarine* s = dynamic_cast<SubMarine *>(item)) {
                s->explose();
                explose();
          }
        }
    }
}

void Bomb::explose()
{
    //here nice explosion
    disconnect(launchAnimation,SIGNAL(finished()),this,SLOT(onAnimationLaunchFinished()));
    launchAnimation->stop();
    launchAnimation->deleteLater();
    launchAnimation = 0;
    destroy();
}

void Bomb::destroy()
{
    if (launchAnimation) {
        disconnect(launchAnimation,SIGNAL(finished()),this,SLOT(onAnimationLaunchFinished()));
        launchAnimation->stop();
        launchAnimation->deleteLater();
    }
    deleteLater();
}
