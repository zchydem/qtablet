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

#include "qstategroup.h"

/*!
    \class QtStateGroup
    \brief The QtStateGroup class manages a mutually exclusive group of states.
    \since 4.5
    \preliminary

    You can make states mutually exclusive by adding them to the same
    QtStateGroup. The state group will ensure that only one state can be active
    at any time, and it also provides an optional default state as a fallback,
    when no other state is active.

    To create a group, simply construct a QtStateGroup object. It's common to
    pass the state owner as the parent for a QtStateGroup (i.e., for two QtState
    objects that share the same widget parent, the QtStateGroup can have the
    same widget parent). Then add states by calling addState(), or remove them
    by calling removeState(). If you want to list all states in the group, you
    can call states(). If you add a QtState, or any QtState subclass as a child
    of a QtStateGroup, it will automatically become a member of that state.

    The group can only have one active state. You can set the active state by
    either calling setActiveState(), or you can call
    QtState::setActive(true) directly on the state itself. When you
    activate a state, QtStateGroup will take care of deactivating the last
    state (if any). If you want to deactivate the active state, you can either
    call setActiveState(0), or QtState::setActive(false). When the
    active state changes, QtStateGroup emits the activeStateChanged() signal.

    Sometimes it's useful to define a "default state" that the group should
    revert to, should any of the other states become deactivated without
    activating another state (e.g., hitting "escape" on a form might revert it
    to its default state, regardless of which state is currently active). You
    can do so by passing a pointer to the default state to setDefaultState().

    \sa QtState, {The Animation Framework}
*/

/*!
    \fn QtStateGroup::activeStateChanged(QtState *state)

    QtStateGroup emits this signal every time the active state changes. \a
    state is the new state, or 0 if there is no active state.

    \sa QtState::setActive()
*/

#include "qstate.h"
#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif
#include "qstate_p.h"

QT_BEGIN_NAMESPACE


#ifdef QT_EXPERIMENTAL_SOLUTION
class QtStateGroupPrivate
#else
class QtStateGroupPrivate : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QtStateGroup)
public:
    QtStateGroupPrivate()
    : activeState(0), defaultState(0)
    { }

    QtState *activeState;
    QtState *defaultState;
    QList<QtState *> states;
#ifdef QT_EXPERIMENTAL_SOLUTION
    QtStateGroup *q_ptr;
#endif
};

/*!
    Constructs a QtStateGroup, passing \a parent to QObject's constructor.
*/
#ifdef QT_EXPERIMENTAL_SOLUTION
QtStateGroup::QtStateGroup(QObject *parent)
    : d_ptr(new QtStateGroupPrivate)
{
    setParent(parent);
    d_ptr->q_ptr = this;
}
#else
QtStateGroup::QtStateGroup(QObject *parent)
    : QObject(*new QtStateGroupPrivate, parent)
{
}
#endif
/*!
    Destroys the QtStateGroup.
*/
QtStateGroup::~QtStateGroup()
{
}

/*!
    Adds \a state to this group. If \a state is already active, it will become this
    group's active state.

    The ownership of \a state is transferred to this group.

    \sa QtState::isActive(), activeState()
*/
void QtStateGroup::addState(QtState *state)
{
    Q_D(QtStateGroup);
    if (!state) {
        qWarning("QtStateGroup::addState: cannot add null state");
        return;
    }
    if (state->group() == this)
        return;
    if (QtStateGroup *other = state->group())
        other->removeState(state);
    d->states << state;
    state->d_func()->group = this;
    state->setParent(this);
    if (state->isActive())
        setActiveState(state);
}

/*!
    Removes \a state from this group. If \a state is the default state, the
    default state will be reset to 0. If \a state is the active state, the
    state will first be deactivated, and then removed from this group. The
    group will then revert to the default state, or 0 if there is no default
    state.

    The ownership of \a state is transferred to the caller (i.e., this
    function does not delete \a state).

    \sa defaultState(), activeState(), QtState::isActive()
*/
void QtStateGroup::removeState(QtState *state)
{
    Q_D(QtStateGroup);
    if (state->group() != this) {
        qWarning("QtStateGroup::removeState: cannot remove state %p,"
                 " which is not part of this group %p",
                 state, this);
        return;
    }
    d->states.removeAll(state);
    state->d_func()->group = 0;
    state->setParent(0);
    if (state == d->defaultState)
        d->defaultState = 0;
    if (state == d->activeState)
        setActiveState(0);
}

/*!
    Returns all states that are part of this group.

    \sa addState(), removeState()
*/
QList<QtState *> QtStateGroup::states() const
{
    Q_D(const QtStateGroup);
    return d->states;
}

/*!
    Returns the default state of the group. The default state is part of this
    group, and is automatically activated if the active state is deactivated.
    If the group does not have a default state, 0 will be returned.
    (e.g., if you call setActiveState(0), or
    QtState::setActive(false)).

    \sa setDefaultState()
*/
QtState *QtStateGroup::defaultState() const
{
    Q_D(const QtStateGroup);
    return d->defaultState;
}

/*!
    Sets \a state, which must be part of this group, to be this group's
    default state. The default state is automatically activated if the active
    state is deactivated (e.g., if you call setActiveState(0), or
    QtState::setActive(false)). If there is no default state when the
    active state is deactivated, then the group's active state will be 0
    (i.e., no state is active).

    \sa defaultState()
*/
void QtStateGroup::setDefaultState(QtState *state)
{
    Q_D(QtStateGroup);
    if (state && state->group() != this) {
        qWarning("QtStateGroup::setDefaultState: cannot set default state %p"
                 " which isn't part of this group.", state);
        return;
    }
    if (state == d->defaultState)
        return;
    d->defaultState = state;
    if (state && !d->activeState)
        state->activate();
}

/*!
    Returns a pointer to the active state if there is an active state;
    otherwise, 0 is returned.

    \sa setActiveState()
*/
QtState *QtStateGroup::activeState() const
{
    Q_D(const QtStateGroup);
    return d->activeState;    
}

/*!
    Sets \a state, which must either be part of this group or 0, to be the
    active state in this group. If \a state is 0, any active state is
    deactivated, and if the group has a default state, the default state is
    automatically activated.

    Calling this function is equivalent to calling QtState::setActive()
    on one of the states in the group.

    \sa activeState(), defaultState()
*/
void QtStateGroup::setActiveState(QtState *state)
{
    Q_D(QtStateGroup);
    if (state && state->group() != this) {
        qWarning("QtStateGroup::setActiveState: state %p belongs"
                 " to group %p; it is not"
                 " part of this group %p",
                 state, state->group(), this);
        return;
    }
    if (state == d->activeState)
        return;
    if (!state && d->defaultState)
        state = d->defaultState;
    QtState *lastState = d->activeState;
    d->activeState = state;
    if (lastState) {
        lastState->d_func()->active = false;
        lastState->leaveState(state);
        emit lastState->deactivated();
        emit lastState->activated(false);
    }
    if (state && !state->isActive()) {
        // Only enter if the state isn't already active. The user might set
        // the state as active, then add it to the group, in which case
        // there's nothing we need to do.
        state->d_func()->active = true;
        state->enterState(lastState);
        emit state->activated();
        emit state->activated(true);
    }
    emit activeStateChanged(state);
}

/*!
    \internal
*/
bool QtStateGroup::event(QEvent *event)
{
    if (event->type() == QEvent::ChildAdded) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (QtState *state = qobject_cast<QtState *>(childEvent->child())) {
            if (state->group() != this)
                addState(state);
        }
    } else if (event->type() == QEvent::ChildRemoved) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (QtState *state = qobject_cast<QtState *>(childEvent->child())) {
            if (state->group() == this)
                removeState(state);
        }
    }
    return QObject::event(event);
}

#include "moc_qstategroup.cpp"

QT_END_NAMESPACE
