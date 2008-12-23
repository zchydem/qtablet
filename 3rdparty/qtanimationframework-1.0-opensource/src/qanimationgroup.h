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

#ifndef QANIMATIONGROUP_H
#define QANIMATIONGROUP_H

#include "qanimation.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QGraphicsWidget;
class QWidget;

class QtAnimationGroupPrivate;

class Q_ANIMATION_EXPORT QtAnimationGroup : public QtAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(Order order READ order)
    Q_PROPERTY(int duration READ duration)

public:
    enum Order {
        Parallel,
        Sequential
    };

    QtAnimationGroup(Order order = Parallel, QObject *parent = 0);
    ~QtAnimationGroup();

    Order order() const;

    QList<QtAbstractAnimation *> animations() const;
    QtAbstractAnimation *at(int index) const;
    int count() const;
    void add(QtAbstractAnimation *animation);
    void addPause(int msecs);
    void insertAt(int index, QtAbstractAnimation *animation);
    void insertPauseAt(int index, int msecs);
    void remove(QtAbstractAnimation *animation);
    void removeAt(int index);
    void clear();

    int duration() const;

    // nesting API
    void beginSequentialGroup();
    void beginParallelGroup();
    void endGroup();

protected:
    QtAnimationGroup(QtAnimationGroupPrivate &dd, Order order, QObject *parent);
    bool event(QEvent *event);

    void updateCurrentTime(int msecs);
    void updateRunning(bool isRunning);

private:
    Q_DECLARE_PRIVATE(QtAnimationGroup)
    Q_PRIVATE_SLOT(d_func(), void _q_animationStarted())
    Q_PRIVATE_SLOT(d_func(), void _q_animationStopped())
    friend class QtTransitionPrivate;
    friend class QtStatePrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
