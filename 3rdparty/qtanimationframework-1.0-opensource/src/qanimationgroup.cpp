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
    \class QtAnimationGroup
    \brief The QtAnimationGroup class provides a sequential or parallel group of animations.
    \since 4.5
    \ingroup animation
    \preliminary

    QtAnimationGroup represents a group of animations, and lets you combine
    sequential and parallel sets of animations into one. The group manages any
    animation that inherits QtAbstractAnimation. By combining groups, you can
    easily construct complex animation graphs.

    To create an animation group, you can start by constructing a
    QtAnimationGroup instance, passing the execution order (Sequential or
    Parallel) to the constructor. Then add animations by calling insertAt() or
    add(). You can remove animations by calling remove(), and clear the
    animation group by calling clear(). QtAnimationGroup takes ownership of the
    animations it manages, and ensures that they are deleted when the
    animation group is deleted.

    For convenience when constructing complex animations, QtAnimationGroup also
    provides the beginSequentialGroup() and beingParallelGroup() functions, as
    well as an endGroup() function.

    You can call addPause() or insertPause() to add a pause to an
    animation. Pauses are most useful for sequential animations.

    \sa QtAbstractAnimation, QtAnimation, QtTransition, {The Animation Framework}
*/

/*!
    \enum QtAnimationGroup::Order

    This enum describes the order of execution for the animations in a
    QtAnimationGroup.

    \value Parallel Parallel execution (default). The animations are all
    started at the same time, and run in parallel. The animation group
    finishes when the longest lasting animation has finished.

    \value Sequential Sequential execution. The first animation in the group
    is started first, and when it stops, the next animation is started, and so
    on. The animation group finishes when the last animation has finished.
*/

#include "qanimation.h"
#include "qanimationgroup.h"
#include <QtCore/qdebug.h>
#include "qanimationgroup_p.h"
#include "qpauseanimation_p.h"

QT_BEGIN_NAMESPACE


int QtAnimationGroupPrivate::indexForTime(int msecs) const
{
    Q_Q(const QtAnimationGroup);
    // ### cache this with pointer and two ints
    Q_ASSERT(order == QtAnimationGroup::Sequential);
    if (msecs == 0 || animations.isEmpty())
        return 0;

    msecs -= currentLoop * q->duration();

    int totalTime = 0;
    for (int i = 0; i < animations.size(); ++i) {
        QtAbstractAnimation *animation = animations.at(i);
        int duration = animation->totalDuration();
        if (duration == -1 || (msecs <= (totalTime + duration)))
            return i;
        totalTime += duration;
    }

    return animations.size() - 1;
}

void QtAnimationGroupPrivate::_q_animationStarted()
{
    Q_Q(QtAnimationGroup);
    QtAbstractAnimation *animation = qobject_cast<QtAbstractAnimation *>(q->sender());
    Q_ASSERT(animation);
    if (animation->duration() == -1 || animation->loopCount() < 0)
        ++uncontrolledAnimations;
}

void QtAnimationGroupPrivate::_q_animationStopped()
{
    Q_Q(QtAnimationGroup);
    QtAbstractAnimation *animation = qobject_cast<QtAbstractAnimation *>(q->sender());
    Q_ASSERT(animation);
    if (animation->duration() == -1 || animation->loopCount() < 0)
        --uncontrolledAnimations;
    if (uncontrolledAnimations <= 0) {
        if (order == QtAnimationGroup::Parallel) {
            int maxDuration = 0;
            for (int i = 0; i < animations.size(); ++i) {
                QtAbstractAnimation *a = animations.at(i);
                if (a->duration() != -1 && a->loopCount() >= 0)
                    maxDuration = qMax(maxDuration, a->totalDuration());
            }
            if (currentTime >= maxDuration) {
                // ### also emit finished!
                q->stop();
            }
        } else {
            if (animation == animations.last()) {
                // ### also emit finished!
                q->stop();
            } else {
                for (int i = 0; i < animations.size(); ++i) {
                    QtAbstractAnimation *a = animations.at(i);
                    if (a == animation) {
                        // ### update internal timer offset
                        animations.at(i + 1)->start();
                    }
                }
            }
        }
    }
}

/*!
    Constructs a QtAnimationGroup, with the given \a order of execution. \a
    parent is passed to QObject's constructor.
*/
QtAnimationGroup::QtAnimationGroup(Order order, QObject *parent) 
    : QtAbstractAnimation(*new QtAnimationGroupPrivate, parent)
{
    Q_D(QtAnimationGroup);
    d->order = order;
}

/*!
    \internal
*/
QtAnimationGroup::QtAnimationGroup(QtAnimationGroupPrivate &dd, Order order, QObject *parent)
    : QtAbstractAnimation(dd, parent)
{
    Q_D(QtAnimationGroup);
    d->order = order;
}

/*!
    Destroys the animation group. It will also destroy all its animations.
*/
QtAnimationGroup::~QtAnimationGroup() 
{
}

/*!
    \property QtAnimationGroup::order
    \brief the animation group's order of execution

    This property describes the animation group's order of execution.  The
    order applies the immediate members of the group. The order is set when
    the animation group is constructed, and cannot be changed.
*/
QtAnimationGroup::Order QtAnimationGroup::order() const
{
    // ### why can't it be changed?
    Q_D(const QtAnimationGroup);
    return d->order;
}

/*!
    Returns the animations that are managed by this group.

    \sa add(), insertAt(), count(), at()
*/
QList<QtAbstractAnimation *> QtAnimationGroup::animations() const
{
    Q_D(const QtAnimationGroup);
    return d->animations;
}

/*!
    Returns the number of animations managed by this group.

    \sa animations(), add(), insertAt(), at()
*/
int QtAnimationGroup::count() const
{
    Q_D(const QtAnimationGroup);
    return d->animations.size();
}

/*!
    Returns a pointer to the animation at \a index in this group. This
    function is useful when you need access to a particular animation.  \a
    index is between 0 and count() - 1.

    \sa count(), animations()
*/
QtAbstractAnimation *QtAnimationGroup::at(int index) const
{
    Q_D(const QtAnimationGroup);
    if (index < 0 || index >= d->animations.size())
        return 0;
    return d->animations.at(index);
}

/*!
    Adds \a animation to this group. This function is equivalent to calling
    insertAt(count(), \a animation).

    \sa insertAt()
*/
void QtAnimationGroup::add(QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);
    insertAt(d->animations.size(), animation);
    // ### what if this is called while the anim is running
}

/*!
    Adds a pause of \a msecs to this animation group.
    The pause is considered as a special type of animation, thus count() will be
    increased by one.
    \sa insertPauseAt(), add(), insertAt()
*/
void QtAnimationGroup::addPause(int msecs)
{
    add(new QPauseAnimation(msecs));
}

/*!
    Inserts \a animation into this animation group at \a index.
    If \a index is 0 the animation is inserted at the beginning.
    If \a index is count(), the animation is inserted at the end.
    \sa removeAt(), add(), animations()
*/
void QtAnimationGroup::insertAt(int index, QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);
    if (!d->nestedGroups.isEmpty()) {
        d->nestedGroups.top()->insertAt(index, animation);
        return;
    }

    d->animations.insert(index, animation);
    if (QtAnimationGroup *oldGroup = animation->d_func()->group)
        oldGroup->remove(animation);
    animation->d_func()->group = this;
    connect(animation, SIGNAL(started()), this, SLOT(_q_animationStarted()));
    animation->setParent(this);
    // ### what if this is called while the anim is running
}

/*!
    Inserts a pause of \a msecs milliseconds at \a index in this animation
    group.

    \sa addPause(), insertAt(), animations()
*/
void QtAnimationGroup::insertPauseAt(int index, int msecs)
{
    insertAt(index, new QPauseAnimation(msecs));
}

/*!
    Removes \a animation from this group. The ownership of \a animation is
    transferred to the caller.

    \sa insertAt(), add()
*/
void QtAnimationGroup::remove(QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);
    if (!animation) {
        qWarning("QtAnimationGroup::remove: cannot remove null animation");
        return;
    }
    int index = d->animations.indexOf(animation);
    if (index == -1) {
        qWarning("QtAnimationGroup::remove: animation is not part of this group");
        return;
    }
    removeAt(index);
}

/*!
    Removes the animation at \a index from this animation group. The ownership
    of the animation is transferred to the caller.

    \sa add(), insertAt()
*/
void QtAnimationGroup::removeAt(int index)
{
    Q_D(QtAnimationGroup);
    if (index < 0 || index >= d->animations.size()) {
        qWarning("QtAnimationGroup::remove: no animation at index %d", index);
        return;
    }
    QtAbstractAnimation *animation = d->animations.at(index);
    animation->d_func()->group = 0;
    // ### animation->setParent(0)
    d->animations.removeAt(index);
    disconnect(animation, SIGNAL(started()), this, SLOT(_q_animationStarted()));
    // ### what if this is called while the anim is running
}

/*!
    Removes all animations from this animation group, and resets the current
    time to 0. The ownership of the animations is transferred to the caller.

    \sa add(), remove()
*/
void QtAnimationGroup::clear()
{
    Q_D(QtAnimationGroup);
    d->animations.clear();
    setCurrentTime(0);
    // ### setParent(0) on each animation.
    // ### what if this is called while the anim is running
}

/*!
    \reimp
*/
int QtAnimationGroup::duration() const
{
    Q_D(const QtAnimationGroup);
    int ret = 0;

    for (int i = 0; i < d->animations.size(); ++i) {
        QtAbstractAnimation *animation = d->animations.at(i);
        const int currentDuration = animation->totalDuration();
        if (currentDuration == -1)
            return -1; // Undetermined length

        if (d->order == Sequential) {
            ret += currentDuration;
        } else { // Parallel
            ret = qMax(ret, currentDuration);
        }
    }

    return ret;
}

/*!
    Begins a new nested sequential group of animations. This convenience
    function makes it easy to construct complex animation groups without
    having to explicitly create a nested animation group object.

    Subsequent calls to add() or insert() will insert animations into the new,
    nested, sequential group. To return to the original group, call
    endGroup().

    \sa beginParallelGroup(), endGroup()
*/
void QtAnimationGroup::beginSequentialGroup()
{
    Q_D(QtAnimationGroup);
    QtAnimationGroup *group = new QtAnimationGroup(QtAnimationGroup::Sequential, this);
    d->nestedGroups.push(group);
}

/*!
    Begins a new nested parallel group of animations. This convenience
    function makes it easy to construct complex animation groups without
    having to explicitly create a nested animation group object.

    Subsequent calls to add() or insert() will insert animations into the new,
    nested, parallel group. To return to the original group, call endGroup().

    \sa beginSequentialGroup(), endGroup()
*/
void QtAnimationGroup::beginParallelGroup()
{
    Q_D(QtAnimationGroup);
    QtAnimationGroup *group = new QtAnimationGroup(QtAnimationGroup::Parallel, this);
    d->nestedGroups.push(group);
}

/*!
    Ends the current nested animation group.

    \sa beginParallelGroup(), beginSequentialGroup()
*/
void QtAnimationGroup::endGroup()
{
    Q_D(QtAnimationGroup);
    if (!d->nestedGroups.isEmpty())
        d->nestedGroups.pop();
}

/*!
    \reimp
*/
bool QtAnimationGroup::event(QEvent *event)
{
    if (event->type() == QEvent::ChildAdded) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (QtAbstractAnimation *a = qobject_cast<QtAbstractAnimation *>(childEvent->child())) {
            if (a->group() != this)
                add(a);
        }
    } else if (event->type() == QEvent::ChildRemoved) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (QtAbstractAnimation *a = qobject_cast<QtAbstractAnimation *>(childEvent->child())) {
            // ### you can only rely on the child being a QObject.  
            // ### This is because in the QEvent::ChildRemoved case it might have already been destructed.
            if (a->group() != this)
                remove(a);
        }
    }
    return QtAbstractAnimation::event(event);
}

/*!
    \reimp
*/
void QtAnimationGroup::updateCurrentTime(int msecs)
{
    Q_D(QtAnimationGroup);
    if (d->animations.isEmpty())
        return;

    if (d->order == Parallel) {
        for (int i = 0; i < d->animations.size(); ++i) {
            QtAbstractAnimation *animation = d->animations.at(i);
            animation->setCurrentTime(msecs);
        }
        return;
    }

    int currentAnimationIndex = d->indexForTime(msecs);
    QtAbstractAnimation *animation = d->animations.at(currentAnimationIndex);
    if (d->lastSequentialAnimationIndex != currentAnimationIndex) {
        if (d->lastSequentialAnimationIndex != -1) {
            if (d->running) {
                d->animations.at(d->lastSequentialAnimationIndex)->stop();
                if (currentAnimationIndex < d->lastSequentialAnimationIndex) {
                    //let's rewind the animations (used when looping
                    for(int i = d->lastSequentialAnimationIndex; i > currentAnimationIndex; --i) {
                        d->animations.at(i)->setCurrentTime(0);
                    }
                } else {
                    //let's fast-forward the animations
                    for(int i = d->lastSequentialAnimationIndex; i < currentAnimationIndex; ++i) {
                        d->animations.at(i)->setCurrentTime(totalDuration());
                    }
                }
            }
            d->lastSequentialAnimationIndex = currentAnimationIndex;
        }
        int tmp = 0;
        for (int i = 0; i < currentAnimationIndex; ++i)
            tmp += d->animations.at(i)->totalDuration();
        d->currentAnimationTimeOffset = tmp + d->currentLoop * duration();
        if (d->running)
            animation->start();
    }

    animation->setCurrentTime(msecs - d->currentAnimationTimeOffset);
}

/*!
    \reimp
*/
void QtAnimationGroup::updateRunning(bool isRunning)
{
    Q_D(QtAnimationGroup);
    if (d->animations.isEmpty())
        return;

    if (isRunning) {
        if (d->order == Parallel) {
            for (int i = 0; i < d->animations.size(); ++i) {
                QtAbstractAnimation *animation = d->animations.at(i);
                animation->start();
            }
        } else {
            int index = d->indexForTime(d->currentTime);
            d->animations.at(index)->start();
        }
    } else {
        if (d->order == Parallel) {
            for (int i = 0; i < d->animations.size(); ++i) {
                QtAbstractAnimation *animation = d->animations.at(i);
                animation->stop();
            }
        } else {
            int index = d->indexForTime(d->currentTime);
            d->animations.at(index)->stop();
        }
    }
    Q_UNUSED(isRunning);
}

#include "moc_qpauseanimation_p.cpp"
#include "moc_qanimationgroup.cpp"

QT_END_NAMESPACE

