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

#ifndef __BOMB__H__
#define __BOMB__H__

#include <QtGui>
#if defined(QT_EXPERIMENTAL_SOLUTION)
# include <common.h>
#endif
#include <qanimationgroup.h>

class PixmapItem;

class Bomb : public QGraphicsWidget
{
Q_OBJECT
public:
    enum Direction {
        Left =  0,
        Right
    };
    Bomb(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    void launch(Direction direction);
    void explose();
    void destroy();

private slots:
    void onAnimationLaunchFinished();
    void onAnimationLaunchValueChanged(const QVariant &);

private:
    QtAnimationGroup *launchAnimation;
    PixmapItem *pixmapItem;
};

#endif //__BOMB__H__
