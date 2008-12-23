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

#ifndef QTRANSITION_H
#define QTRANSITION_H

#include "qanimationgroup.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QtState;

class QtTransitionPrivate;
class Q_ANIMATION_EXPORT QtTransition : public QtAnimationGroup
{
    Q_OBJECT
    Q_PROPERTY(QtState* fromState READ fromState WRITE setFromState NOTIFY fromStateChanged)
    Q_PROPERTY(QtState* toState READ toState WRITE setToState NOTIFY toStateChanged)

public:
    explicit QtTransition(QObject *parent = 0);
    explicit QtTransition(QtState *fromState, QtState *toState = 0, QObject *parent = 0);
    ~QtTransition();

    QtState *fromState() const;
    void setFromState(QtState *state);

    QtState *toState() const;
    void setToState(QtState *state);

private:
    Q_DISABLE_COPY(QtTransition)
    Q_DECLARE_PRIVATE(QtTransition)
    Q_PRIVATE_SLOT(d_func(), void _q_stateRemoved())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
