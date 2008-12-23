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

/*!
    \class QtTransition
    \brief The QtTransition class describes transitions between QtStates.
    \ingroup animation
    \since 4.5
    \preliminary

    QtTransition is a special animation group that describes how certain
    properties of objects should change as one state is activated or
    deactivated, or when switching between states. The API is quite simple; in
    addition to QtAnimationGroup's API, it adds only two properties: fromState,
    and toState, of which both can be 0 (indicating "any state"). Combining
    the two properties will allow you to choose between four contexts for this
    transition:

    \list
    \o From any state to any state
    \o From state A to any state 
    \o From any state to state B
    \o From state A to state B
    \endlist

    To define a transition, you start by constructing a QtTransition object,
    passing a pointer to the fromState and toState to QtTransition's
    constructor. Then add animations to the transition just like you would add
    animations to any animation group. What's different from QtAnimationGroup
    is that you usually do not specifiy neither start nor target values for
    the animations, as these are automatically initialized from the states you
    are transitioning from and to.

    The transition is managed by QtState. As you enter a state, QtState will
    initialize and activate all transitions that apply to the activated and
    deactivated states. The initialization step is what makes QtTransition a
    special class: before the transition is started, all animations whose
    target objects and properties are defined in fromState and toState are
    initialized with the respective start and target values. The first
    animation in this transition for any object and property is initialized
    with the property values from fromState (or with the object's current
    property value if fromState is either 0, or if fromState does not define
    any property value for that object). The last animation that defines
    properties for objects are initialized with the target value from toState,
    or with the implicit default values from the state that was left if
    toState does not define properties for the particular object.

    This allows QtTransition to define arbitrarily complex transitions for all
    properties between states, including sequential transitions where
    properties change from a implicit start value, to a sequence of defined
    values, and finally an implicit target value.

    \sa QtState, QtAnimation, {The Animation Framework}
*/

#include "qtransition.h"

#include "qanimation.h"
#include "qstate.h"
#include <QtCore/qdebug.h>
#include <QtCore/qmap.h>
#include <QtGui/qgraphicswidget.h>
#include "qanimationgroup_p.h"
#include "qanimationpointer_p.h"
#include "qstate_p.h"
#include "qtransition_p.h"

QT_BEGIN_NAMESPACE

void QtTransitionPrivate::_q_stateRemoved()
{
    Q_Q(QtTransition);
    QtState *removedState = (QtState *)q->sender();
    if (removedState == fromState)
        fromState = 0;
    else if (removedState == toState)
        toState = 0;
}

/*!
    Constructs a QtTransition instance. \a parent is passed to
    QtAnimationGroup's constructor. QtTransition is a parallel animation group.
    
    The properties fromState and toState will be set to 0, causing the 
    transition to activate when leaving from any state, and entering any state.
*/
QtTransition::QtTransition(QObject *parent)
    : QtAnimationGroup(*new QtTransitionPrivate, Parallel, parent)
{
    QtStatePrivate::anyTransitions << this;
}

/*!
    Constructs a QtTransition instance between \a fromState and \a toState. \a
    parent is passed to QtAnimationGroup's constructor. QtTransition is a
    parallel animation group.

    If \a fromState is 0, the transition will activate when entering \a
    toState from any state. Similarily, if \a toState is 0, the transition
    will activate when leaving \a fromState, and entering any state.
*/
QtTransition::QtTransition(QtState *fromState, QtState *toState, QObject *parent)
    : QtAnimationGroup(*new QtTransitionPrivate, Parallel, parent)
{
    setFromState(fromState);
    setToState(toState);
    if (!fromState && !toState)
        QtStatePrivate::anyTransitions << this;
}

/*!
    Destroys the transition.
*/
QtTransition::~QtTransition()
{
}

/*!
    \property QtTransition::fromState
    \brief the from-state for the transition

    This property describes the transition's from-state, or starting state.
    This state will initialize all the transition's animations whose start
    values are undefined.

    QtTransition does not take ownership of the state.

    \sa toState
*/
QtState *QtTransition::fromState() const
{
    Q_D(const QtTransition);
    return d->fromState;
}
void QtTransition::setFromState(QtState *state)
{
    Q_D(QtTransition);
    if (state && d->toState) {
        if (state->group() != d->toState->group() || !state->group()) {
            qWarning("QtTransition::setFromState: cannot define transition"
                     " between states that are not part of the same group");
            return;
        }
        if (state == d->toState) {
            qWarning("QtTransition::setFromState: cannot define transition"
                     " from and to the same state");
            return;
        }
    }
    if (d->fromState == state)
        return;
    if (d->fromState)
        d->fromState->d_func()->leaveTransitions.remove(this);
    QtState *lastFromState = d->fromState;
    if ((d->fromState = state)) {
        QtStatePrivate::anyTransitions.remove(this);
        state->d_func()->leaveTransitions << this;
        connect(state, SIGNAL(destroyed()), this, SLOT(_q_stateRemoved()));
    } else if (!d->toState) {
        QtStatePrivate::anyTransitions << this;
        if (lastFromState)
            disconnect(lastFromState, SIGNAL(destroyed()), this, SLOT(_q_stateRemoved()));
    }
}

/*!
    \property QtTransition::toState
    \brief the to-state for the transition

    This property describes the transition's to-state, or target state.  This
    state will initialize all the transition's animations whose target values
    are undefined.

    QtTransition does not take ownership of the state.

    \sa fromState
*/
QtState *QtTransition::toState() const
{
    Q_D(const QtTransition);
    return d->toState;
}
void QtTransition::setToState(QtState *state)
{
    Q_D(QtTransition);
    if (state && d->fromState) {
        if (state->group() != d->fromState->group() || !state->group()) {
            qWarning("QtTransition::setToState: cannot define transition"
                     " between states that are not part of the same group");
            return;
        }
        if (state == d->fromState) {
            qWarning("QtTransition::setToState: cannot define transition"
                     " from and to the same state");
            return;
        }
    }
    if (d->toState == state)
        return;
    if (d->toState)
        d->toState->d_func()->enterTransitions.remove(this);
    QtState *lastToState = d->toState;
    if ((d->toState = state)) {
        QtStatePrivate::anyTransitions.remove(this);
        state->d_func()->enterTransitions << this;
        connect(state, SIGNAL(destroyed()), this, SLOT(_q_stateRemoved()));
    } else {
        QtStatePrivate::anyTransitions << this;
        if (lastToState)
            disconnect(lastToState, SIGNAL(destroyed()), this, SLOT(_q_stateRemoved()));
    }
}

QT_END_NAMESPACE

#include "moc_qtransition.cpp"
