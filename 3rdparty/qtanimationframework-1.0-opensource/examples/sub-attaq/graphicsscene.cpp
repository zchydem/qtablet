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
#include "graphicsscene.h"
#include "boat.h"
#include "submarine.h"
#include "pixmapitem.h"

GraphicsScene::GraphicsScene(int x, int y, int width, int height, Mode mode)
    : QGraphicsScene(x,y,width,height), currentLevel(1), gameIsRunning(false), mode(mode)
{
    backgroundItem = new PixmapItem(QString("background"),mode);
    backgroundItem->setZValue(1);
    backgroundItem->setPos(0,0);
    addItem(backgroundItem);

    PixmapItem *surfaceItem = new PixmapItem(QString("surface"),mode);
    surfaceItem->setZValue(3);
    surfaceItem->setPos(0,sealLevel() - surfaceItem->boundingRect().height()/2);
    addItem(surfaceItem);
}

GraphicsScene::~GraphicsScene()
{
}

bool GraphicsScene::gameInProgress() const
{
     return gameIsRunning;
}

float GraphicsScene::sealLevel() const
{
     if (mode == Big)
        return 220;
     else
        return 160;
}

void GraphicsScene::initializeLevel()
{
    boat = new Boat();
    addItem(boat);
    setFocusItem(boat,Qt::OtherFocusReason);
    boat->setPos(width()/2, sealLevel() - boat->size().height());
    connect(boat, SIGNAL(destroyed(QObject *)), this, SLOT(onBoatDestroy(QObject *)));
    gameIsRunning = true;

    for (int i = 0 ; i <= 5 ; ++i)
        subMarines.insert(new SubMarine());

    foreach (SubMarine *sub,subMarines) {
        addItem(sub);
        connect(sub,SIGNAL(destroyed(QObject *)), this, SLOT(onSubMarineDestroy(QObject *)));
        int random = (qrand() % 15 + 1);
        qreal x = random == 13 || random == 5 ? 0 : width() - sub->size().width();
        qreal y = height() -(qrand() % 150 + 1) - sub->size().height();
        sub->setPos(x,y);
        sub->setCurrentDirection(x == 0 ? SubMarine::Right : SubMarine::Left);
        sub->setCurrentSpeed(qrand() % 3 + 1);
    }
}

void GraphicsScene::pause()
{
    //### TODO
}

void GraphicsScene::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    event->ignore();
}

void GraphicsScene::onSubMarineDestroy(QObject * obj)
{
    subMarines.remove(static_cast<SubMarine *>(obj));
    if (subMarines.size() == 0 && gameIsRunning) {
        qDebug()<<"You win!!!!!!!!!!!!";
        gameIsRunning = false;
        disconnect(boat, SIGNAL(destroyed(QObject *)), this, SLOT(onBoatDestroy(QObject *)));
        boat->destroy();
    }
}

void GraphicsScene::onBoatDestroy(QObject *)
{
    qDebug()<<"You Loose :( :(";
    gameIsRunning = false;
    foreach (SubMarine *sub,subMarines) {
        sub->destroy();
    }
}
