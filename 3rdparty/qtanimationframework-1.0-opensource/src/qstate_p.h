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

#ifndef QSTATE_P_H
#define QSTATE_P_H

#include "qstate.h"

#include <QtCore/qset.h>

#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif
#include "qconnectionstate_p.h"
#include "qpropertystate_p.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtState. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QtAbstractAnimation;
class QtAnimation;
class QtAnimationGroup;
class QtTransition;

#ifdef QT_EXPERIMENTAL_SOLUTION
class QtStatePrivate
#else
class QtStatePrivate : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QtState)
public:
    QtStatePrivate()
        : active(false),
          restoreProperties(true),
          q_ptr(0),
          group(0)
    { }

    bool active;
    bool restoreProperties;
    QtState *q_ptr;
    QtStateGroup *group;

    static void initializeStartPropertyHelper(QtAbstractAnimation *animation,
                                              QMultiMap<QtAnimationPointer, QByteArray> *data);
    static void initializeTargetPropertyHelper(QtAbstractAnimation *animation,
                                               QMap<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> *data);
    static void initializeTransition(QtTransition *transition, QtState *fromState, QtState *toState);
    
    void dig(QtAnimationGroup *group, QSet<QPair<QtAnimationPointer, QByteArray> > *props) const;
    void clear_helper();
    QSet<QtTransition *> enterTransitions;
    QSet<QtTransition *> leaveTransitions;
    static QSet<QtTransition *> anyTransitions;
#ifndef QT_NO_PROPERTIES
    QList<QPropertyState *> propertyStates;
    QMap<QPair<QtAnimationPointer, QByteArray>, QPropertyState *> oldPropertyStates;
#endif
    QList<QConnectionState *> connectionStates;
};

QT_END_NAMESPACE

#endif
