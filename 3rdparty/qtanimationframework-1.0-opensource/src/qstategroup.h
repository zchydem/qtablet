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

#ifndef QSTATEGROUP_H
#define QSTATEGROUP_H

#include <QtCore/qobject.h>
#include <QtCore/qmetatype.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "common.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QtState;

class QtStateGroupPrivate;
class Q_ANIMATION_EXPORT QtStateGroup : public QObject
{
    Q_OBJECT
public:
    QtStateGroup(QObject *parent = 0);
    ~QtStateGroup();

    void addState(QtState *state);
    void removeState(QtState *state);
    QList<QtState *> states() const;

    QtState *defaultState() const;
    void setDefaultState(QtState *state);

    QtState *activeState() const;
    void setActiveState(QtState *state);

Q_SIGNALS:
    void activeStateChanged(QtState *state);

protected:
    bool event(QEvent *event);

private:
    Q_DECLARE_PRIVATE(QtStateGroup)
    Q_DISABLE_COPY(QtStateGroup)
#ifdef QT_EXPERIMENTAL_SOLUTION
    QtStateGroupPrivate *d_ptr;
#endif
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QtStateGroup *)

QT_END_HEADER

#endif
