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

#ifndef QABSTRACTANIMATION_H
#define QABSTRACTANIMATION_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QtAnimationGroup;
class QtAbstractAnimationPrivate;
class QtAnimationPrivate;
class QtAnimationGroupPrivate;

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "common.h"
#endif

class Q_ANIMATION_EXPORT QtAbstractAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY running)
    Q_PROPERTY(qreal loopCount READ loopCount WRITE setLoopCount)
    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime)
    Q_PROPERTY(int currentLoop READ currentLoop NOTIFY currentLoopChanged)

public:
    enum DeletionPolicy {
        KeepWhenFinished = 0,
        DeleteWhenFinished
    };

    QtAbstractAnimation(QObject *parent = 0);
    virtual ~QtAbstractAnimation();

    bool isRunning() const;

    QtAnimationGroup *group() const;

    qreal loopCount() const;
    void setLoopCount(qreal loopCount);
    int currentLoop() const;

    virtual int duration() const = 0;
    int totalDuration() const;

    int currentTime() const;

Q_SIGNALS:
    void started();
    void stopped();
    void finished();
    void running(bool isRunning);
    void currentLoopChanged(int currentLoop);

public Q_SLOTS:
    void start(DeletionPolicy policy = KeepWhenFinished);
    void stop();
    void setCurrentTime(int msecs);

protected:
    QtAbstractAnimation(QtAbstractAnimationPrivate &dd, QObject *parent = 0);
    bool event(QEvent *event);

    virtual void updateCurrentTime(int msecs) = 0;
    virtual void updateRunning(bool isRunning);

#ifdef QT_EXPERIMENTAL_SOLUTION
     QtAbstractAnimationPrivate *d_ptr;
#endif

private:
    Q_DISABLE_COPY(QtAbstractAnimation)
    Q_DECLARE_PRIVATE(QtAbstractAnimation)

    friend class QUnifiedTimer;
    friend class QtAnimation;
    friend class QtAnimationPrivate;
    friend class QtAnimationGroup;
    friend class QtAnimationGroupPrivate;
    friend class QtState;
    friend class QtStatePrivate;
};
QT_END_NAMESPACE

QT_END_HEADER

#endif // QANIMATION_H
