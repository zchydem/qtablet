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

#ifndef QANIMATIONGROUP_P_H
#define QANIMATIONGROUP_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtAnimationGroup. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qanimationgroup.h"

#include <QtCore/qlist.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qstack.h>

#include "qanimation_p.h"

QT_BEGIN_NAMESPACE

class QtAnimationGroupPrivate : public QtAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QtAnimationGroup);
public:
    QtAnimationGroupPrivate()
        : lastSequentialAnimationIndex(0),
          currentAnimationTimeOffset(0),
          order(QtAnimationGroup::Parallel),
          uncontrolledAnimations(0)
    { }

    QList<QtAbstractAnimation *> animations;
    int lastSequentialAnimationIndex;
    int currentAnimationTimeOffset;

    int indexForTime(int msecs) const;

    // for the convenience nesting API
    QStack<QtAnimationGroup *> nestedGroups;

    QtAnimationGroup::Order order;

    // private slots
    int uncontrolledAnimations;
    void _q_animationStarted();
    void _q_animationStopped();
};

QT_END_NAMESPACE

#endif //QANIMATIONGROUP_P_H
