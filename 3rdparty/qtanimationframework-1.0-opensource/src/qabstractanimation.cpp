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
    \class QtAbstractAnimation
    \ingroup animation
    \brief The QtAbstractAnimation class provides an abstract base class for animations.
    \since 4.5
    \preliminary

    This class is part of {The Animation Framework}. It serves as a base class
    for standard animations and groups, with functions for shared
    functionality, and it also makes it easy for you to define custom
    animations that plug into the rest of the animation framework.

    If you want to create an animation, you should look at the two subclasses,
    QtAnimation and QtAnimationGroup, instead.

    QtAbstractAnimation provides an interface for the current time and
    duration, the loop count, and state of an animation. These properties
    define the base functionality common to all animations in Qt. The virtual
    duration() function returns the local duration of the animation; i.e., for
    how long the animation should update the current time before
    looping. Subclasses can implement this function differently; for example,
    QtAnimation returns the duration of a simple animated property, whereas
    QtAnimationGroup returns the duration of a set or sequence of
    animations. You can also set a loop count by calling setLoopCount(); a
    loop count of 2 will let the animation run twice (the default value is
    1). Since the loop count is a qreal, you can define a loop count of 0.5,
    in which cause the animation will stop when it has reached 50% of its
    duration.

    Like QTimeLine, QtAbstractAnimation also provides an interface for starting
    and stopping an animation, and for tracking its progress. You can call the
    start() slot to start the animation. When the animation starts, the
    started() signal is emitted, and isRunning() returns true. If you call the
    stop() slot, the stopped() signal is emitted, and isRunning() returns
    false. If the animation reaches the end, the finished() signal is
    emitted. You can check the current state by calling isRunning().

    QtAbstractAnimation provides two functions that are pure virtual, and must
    be reimplemented in a subclass: duration(), and updateCurrentTime(). The
    duration() function lets you report a duration for the animation (a return
    value of -1 signals that the animation runs forever until explicitly
    stopped). The current time is delivered by the framework through calls to
    updateCurrentTime(). By reimplementing this function, you can track the
    animation progress and update your target objects accordingly. By
    reimplementing updateRunning(), you can track the animation's state
    changes, which is particularily useful for animations that are not driven
    by time.

    \sa QtAnimation, QtAnimationGroup, {The Animation Framework}
*/

/*!
    \enum QtAbstractAnimation::DeletionPolicy

    \value KeepWhenFinished The animation will not be deleted when finished.
    \value DeleteWhenFinished The animation will be automatically deleted when
    finished.
*/

/*!
    \fn QtAbstractAnimation::started()

    QtAbstractAnimation emits this signal when the animation is started. Once
    control reaches the event loop, the animation will run by itself and
    periodically call the currentTimeChanged() slot.

    This signal is emitted after running(bool), but before the virtual
    updateRunning() function is called.

    \sa stopped()
*/

/*!
    \fn QtAbstractAnimation::stopped()

    QtAbstractAnimation emits this signal when the animation has stopped.

    This signal is emitted after running(bool).

    \sa started(), running()
*/

/*!
    \fn QtAbstractAnimation::finished()

    QtAbstractAnimation emits this signal after the animation has stopped and
    has reached the end.

    This signal is emitted after stopped().

    \sa started(), running(), stopped()
*/

/*!
    \fn QtAbstractAnimation::running(bool isRunning)

    QtAbstractAnimation emits this signal whenever the animation has started or
    stopped.  If \a isRunning is true, the animation has started; otherwise it
    has stopped.

    \sa started(), stopped(), updateRunning()
*/

/*!
    \fn QtAbstractAnimation::currentLoopChanged(int currentLoop)

    QtAbstractAnimation emits this signal whenever the current loop changes. \a
    currentLoop is the current loop.

    \sa currentLoop(), loopCount()
*/

#include "qabstractanimation.h"
#include "qanimationgroup.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
#include <QtGui/qcolor.h>

#include "qabstractanimation_p.h"

#include <qmath.h>

///TODO: Remove QUnifiedTimer once Qt provides the functionality

#define TIMER_INTERVAL 16

QT_BEGIN_NAMESPACE


class QUnifiedTimer : public QObject
{
private:
    QUnifiedTimer(QObject *parent)
        : QObject(parent),
          timer(0),
          lastTick(0)
    {
    }

public:
    static QUnifiedTimer *instance()
    {
        static QUnifiedTimer *inst = new QUnifiedTimer(qApp);
        return inst;
    }

    void timerEvent(QTimerEvent *)
    {
        lastTick = time.elapsed();

        for (int i = 0; i < animations.size(); ++i) {
            QtAbstractAnimation *animation = animations.at(i);
            int elapsed = lastTick - animation->d_func()->unifiedTimerOffset;
            animation->setCurrentTime(elapsed);
        }
    }

    void updateTimer()
    {
        if (animations.isEmpty() && timer) {
            killTimer(timer);
            timer = 0;
            lastTick = 0;
            time = QTime();
        } else if (!animations.isEmpty() && !timer) {
            timer = startTimer(TIMER_INTERVAL);
            lastTick = 0;
            time.start();
        }
    }

    void registerAnimation(QtAbstractAnimation *animation)
    {
        if (animations.contains(animation))
            return;
        animation->d_func()->unifiedTimerOffset = lastTick - animation->currentTime();
        animations << animation;
        updateTimer();
    }

    void unregisterAnimation(QtAbstractAnimation *animation)
    {
        animations.removeAll(animation);
        updateTimer();
    }

private:
    int timer;
    QTime time;
    int lastTick;
    QList<QtAbstractAnimation *> animations;
};

void QtAbstractAnimationPrivate::setRunning(bool enabled)
{
    Q_Q(QtAbstractAnimation);
    if (running == enabled)
        return;
    running = enabled;

    // Notify state change
    int dura = q->duration();
    emit q->running(running);
    if (running) {
        emit q->started();
    } else {
        emit q->stopped();
        if (dura != -1 && (currentTime * (currentLoop + 1)) == (dura * loopCount)) {
            emit q->finished();
            if (deleteWhenFinished)
                q->deleteLater();
        }
    }

    q->updateRunning(running);

    // Enter running state.
    if (running) {
        // Rewind if we're at the end when starting.
        if (dura != -1 && currentTime == dura && (loopCount < 0 || currentLoop == (loopCount - 1)))
            q->setCurrentTime(0);

        // Register timer if we're the toplevel that's running.
        QtAnimationGroup *p = q->group();
        bool parentGroupIsRunning = false;
        while (p) {
            if (p->isRunning()) {
                parentGroupIsRunning = true;
                break;
            }
            p = p->group();
        }
        if (!parentGroupIsRunning) {
            isRegisteredWithUnifiedTimer = true;
            QUnifiedTimer::instance()->registerAnimation(q);
        }
    } else {
        // Leave running state.
        if (isRegisteredWithUnifiedTimer)
            QUnifiedTimer::instance()->unregisterAnimation(q);
    }
}

/*!
    Constructs the QtAbstractAnimation base class, and passes \a parent to
    QObject's constructor.

    \sa QtAnimation, QtAnimationGroup
*/
#ifdef QT_EXPERIMENTAL_SOLUTION
QtAbstractAnimation::QtAbstractAnimation(QObject *parent)
    : d_ptr(new QtAbstractAnimationPrivate)
{
    // Allow auto-add on reparent
    setParent(parent);
    d_ptr->q_ptr = this;
}
#else
QtAbstractAnimation::QtAbstractAnimation(QObject *parent)
    : QObject(*new QtAbstractAnimationPrivate, 0)
{
    // Allow auto-add on reparent
    setParent(parent);
}
#endif

/*!
    \internal
*/
#ifdef QT_EXPERIMENTAL_SOLUTION
QtAbstractAnimation::QtAbstractAnimation(QtAbstractAnimationPrivate &dd, QObject *parent)
    : d_ptr(&dd)
{
    // Allow auto-add on reparent
   setParent(parent);
   d_ptr->q_ptr = this;
}
#else
QtAbstractAnimation::QtAbstractAnimation(QtAbstractAnimationPrivate &dd, QObject *parent)
    : QObject(dd, 0)
{
    // Allow auto-add on reparent
   setParent(parent);
}
#endif

/*!
    Stops the animation if it's running, then destroys the
    QtAbstractAnimation. If the animation is part of a QtAnimationGroup, it is
    automatically removed before it's destroyed.
 */
QtAbstractAnimation::~QtAbstractAnimation()
{
}

/*!
    \property QtAbstractAnimation::running
    \brief whether the animation is running or not.

    This property describes the current state of the animation. When the
    animation state changes, QtAbstractAnimation emits the running() signal.

    \sa QTimeLine::state()
 */
bool QtAbstractAnimation::isRunning() const
{
    Q_D(const QtAbstractAnimation);
    return d->running;
}

/*!
    If this animation is part of a QtAnimationGroup, this function returns a
    pointer to the group; otherwise, it returns 0.

    \sa QtAnimationGroup::add()
*/
QtAnimationGroup *QtAbstractAnimation::group() const
{
    Q_D(const QtAbstractAnimation);
    return d->group;
}

/*!
    \property QtAbstractAnimation::loopCount
    \brief the loop count of the animation

    This property describes the loop count of the animation as a qreal (float
    or double). By default this value is 1, indicating that the animation
    should run once only, and then stop. By changing it you can let the
    animation loop several times. With a value of 0, the animation will not
    run at all, and with a negative value, the animation will loop forever
    until stopped.

    Since the value is a qreal, you are free to set loop counts of 1.5 or 2.5.
*/
qreal QtAbstractAnimation::loopCount() const
{
    Q_D(const QtAbstractAnimation);
    return d->loopCount;
}
void QtAbstractAnimation::setLoopCount(qreal loopCount)
{
    Q_D(QtAbstractAnimation);
    d->loopCount = loopCount;
}

/*!
    \property QtAbstractAnimation::currentLoop
    \brief the current loop of the animation

    This property describes the current loop of the animation. By default, the
    animation's loop count is 1, and so the current loop will always be 0. If
    the loop count is 2 and the animation runs past its duration, it will
    automatically rewind and restart at current time 0, and current loop 1,
    and so on.

    When the current loop changes, QtAbstractAnimation emits the
    currentLoopChanged() signal.
*/
int QtAbstractAnimation::currentLoop() const
{
    Q_D(const QtAbstractAnimation);
    return d->currentLoop;
}

/*!
    \fn virtual int QtAbstractAnimation::duration() const = 0

    This pure virtual function returns the duration of the animation, and
    defines for how long QtAbstractAnimation should update the current
    time. This duration is local, and does not include the start delay or loop
    counts.

    A return value of -1 indicates that the animation has no defined duration;
    the animation should run forever until stopped. This is useful for
    animations that are not time driven, or where you cannot easily predict
    its duration (e.g., event driven audio playback in a game).

    If the animation is a parallel QtAnimationGroup, the duration will be the longest
    duration of all its animations. If the animation is a sequential QtAnimationGroup,
    the duration will be the sum of the duration of all its animations.
    \sa loopCount
*/

/*!
    Returns the total and effective duration of the animation, including the
    loop count.

    \sa duration(), currentTime
*/
int QtAbstractAnimation::totalDuration() const
{
    Q_D(const QtAbstractAnimation);
    if (d->loopCount < 0)
        return -1;
    int dura = duration();
    if (dura == -1)
        return -1;
    return dura * d->loopCount;
}

/*!
    \property QtAbstractAnimation::currentTime
    \brief the current time and progress of the animation

    This property describes the animation's current time. You can change the
    current time by calling setCurrentTime, or you can call start() and let
    the animation run, setting the current time automatically as the animation
    progresses.

    The animation's current time starts at 0, and ends at duration(). If the
    animation's loopCount is larger than 1, the current time will rewind and
    start at 0 again for the consecutive loops. If the animation has a pause.
    currentTime will also include the duration of the pause.

    \sa loopCount
 */
int QtAbstractAnimation::currentTime() const
{
    Q_D(const QtAbstractAnimation);
    return d->currentTime;
}
void QtAbstractAnimation::setCurrentTime(int msecs)
{
    Q_D(QtAbstractAnimation);
    if (msecs < 0)
        return;

    // Calculate new time and loop.
    int dura = duration();
    int totalDura = (d->loopCount < 0.0 || dura == -1) ? -1 : int(dura * d->loopCount);
    if (totalDura != -1)
        msecs = qMin(totalDura, msecs);
    int newTime = dura == -1 ? msecs : (msecs ? (((msecs - 1) % dura) + 1) : 0);
    int newLoop = dura == -1 ? 0 : (msecs ? ((msecs - 1) / dura) : 0);
    if (newTime == d->currentTime && newLoop == d->currentLoop)
        return;

    // Update new values.
    int oldLoop = d->currentLoop;
    d->currentTime = newTime;
    d->currentLoop = newLoop;
    updateCurrentTime(msecs);
    if (newLoop != oldLoop)
        emit currentLoopChanged(newLoop);

    // All animations are responsible for stopping the animation when their
    // own end state is reached; in this case the animation is time driven,
    // and has reached the end.
    if (totalDura != -1 && msecs == totalDura)
        stop();
}

/*!
    Starts the animation. The \a policy argument says whether or not the
    animation should be deleted when it's done. When the animation starts, the
    started() signal is emitted, and isRunning() returns true. When control
    reaches the event loop, the animation will run by itself, periodically
    calling updateCurrentTime() as the animation progresses.

    If the animation is currently stopped when you call start(), it is resumed
    and continues from where it left off. When the animation reaches the end,
    the animation will either stop, or if the loop level is more than 1, it
    will rewind and continue from the beginning. If you start the animation
    after it has reached the end, the animation will rewind before starting
    again.

    If the animation is already running, this function does nothing.

    \sa stop(), isRunning()
*/
void QtAbstractAnimation::start(DeletionPolicy policy)
{
    Q_D(QtAbstractAnimation);
    d->setRunning(true);
    d->deleteWhenFinished = policy;
}

/*!
    Stops the animation. When the animation is stopped, it emits the stopped()
    signal, and isRunning() returns false. The current time is not changed.

    If the animation stops by itself after reaching the end (i.e.,
    currentTime() == duration() and currentLoop() > loopCount() - 1), the
    finished() signal is emitted.

    \sa start(), isRunning()
 */
void QtAbstractAnimation::stop()
{
    Q_D(QtAbstractAnimation);
    d->setRunning(false);
}

/*!
    \reimp
*/
bool QtAbstractAnimation::event(QEvent *event)
{
    return QObject::event(event);
}

/*!
    \fn virtual void QtAbstractAnimation::updateCurrentTime(int msecs) = 0;

    This pure virtual function is called every time the animation's current
    time changes. The \a msecs argument is the current time.

    \sa updateRunning()
*/

/*!
    This virtual function is called by QtAbstractAnimation when the animation
    starts and stops. If \a running is true, the animation has started; otherwise
    it has stopped.

    \sa start(), stop(), updateCurrentTime()
*/
void QtAbstractAnimation::updateRunning(bool running)
{
    Q_UNUSED(running);
}

#include "moc_qabstractanimation.cpp"

QT_END_NAMESPACE

