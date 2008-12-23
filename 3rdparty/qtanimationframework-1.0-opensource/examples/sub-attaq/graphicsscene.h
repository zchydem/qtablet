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

#ifndef __GRAPHICSSCENE__H__
#define __GRAPHICSSCENE__H__

#include <QtGui>

class Boat;
class SubMarine;
class PixmapItem;

class GraphicsScene : public QGraphicsScene
{
Q_OBJECT
public:
    enum Mode {
        Big = 0,
        Small
    };
    GraphicsScene(int x, int y, int width, int height, Mode mode = Big);
    ~GraphicsScene();
    bool gameInProgress() const;
    float sealLevel() const;
    void initializeLevel();
    void pause();

protected:
    void mousePressEvent (QGraphicsSceneMouseEvent * event);

private slots:
    void onBoatDestroy(QObject *);
    void onSubMarineDestroy(QObject *);

private:
    int currentLevel;
    bool gameIsRunning;
    Mode mode;
    PixmapItem *backgroundItem;
    Boat *boat;
    QSet<SubMarine *> subMarines;
};

#endif //__GRAPHICSSCENE__H__

