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

#ifndef QABSTRACTANIMATION_P_H
#define QABSTRACTANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtAbstractAnimation. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbasictimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>
#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif

QT_BEGIN_NAMESPACE

class QtAnimationGroup;
class QtAbstractAnimation;
#ifdef QT_EXPERIMENTAL_SOLUTION
class QtAbstractAnimationPrivate
#else
class QtAbstractAnimationPrivate : public QObjectPrivate
#endif
{
public:
    QtAbstractAnimationPrivate()
        : running(false),
          deleteWhenFinished(false),
          currentTime(0), 
          loopCount(1.),
          currentLoop(0),
          startTime(0),
          group(0),
          isRegisteredWithUnifiedTimer(false),
          unifiedTimerOffset(0)
    {
    }

    bool running;
    bool deleteWhenFinished;
    void setRunning(bool enabled);

    int currentTime;
    qreal loopCount;
    qreal currentLoop;

    // for the group
    int startTime;

    QtAnimationGroup *group;
#ifdef QT_EXPERIMENTAL_SOLUTION
    QtAbstractAnimation *q_ptr;
#endif
    //for the timer
    bool isRegisteredWithUnifiedTimer;
    int unifiedTimerOffset;

private:
    friend class UnifiedTimer;
    friend class QAbstractAnimator;
    Q_DECLARE_PUBLIC(QtAbstractAnimation)
};
QT_END_NAMESPACE
#endif
