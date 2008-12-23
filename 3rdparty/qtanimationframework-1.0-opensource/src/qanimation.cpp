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
    \class QtAnimation
    \brief The QtAnimation class animates properties for QObject, QWidget, and
    QGraphicsItem
    \since 4.5
    \ingroup animation
    \preliminary

    This class is part of {The Animation Framework}.  You can use QtAnimation
    by itself as a simple animation class, or as part of more complex
    animations through QtAnimationGroup or QtTransition.

    The most common way to use QtAnimation is to construct an instance of it by
    passing a pointer to a QObject, a QWidget or a QGraphicsItem, and the name
    of the property you would like to animate, to QtAnimation's
    constructor. You can then set start and target values for the property by
    calling setStartValue() and setTargetValue(), and finally call start() to
    start the animation. When control goes back to the event loop, QtAnimation
    will interpolate the property of the target object, widget or item.

    There are two ways to affect how QtAnimation interpolates the values. You
    can set an easing curve by calling setEasingCurve(), and configure the
    duration by calling setDuration(). You can change how the QVariants are
    interpolated by creating a subclass of QtAnimation, and reimplementing the
    virtual interpolated() function.

    The start value of the animation is optional. If you do not set any start
    value, the animation will operate on the target's current property value
    at the point when the animation was started. You can call setStartValue()
    to set the start value, and setTargetValue() to set the target value for
    the animated property. When used as part of a QtTransition, it's common to
    omit both the start and target values, as these are set automatically by
    the transition as it is initialized.

    Animations can operate on QObjects, QWidgets and
    QGraphicsItems. QGraphicsItem does not inherit QObject, and does not
    provide any properties as such. For convenience however, QtAnimation
    emulates properties for QGraphicsItem, so that you can animate an item's
    position, geometry or opacity, just like with QObjects. You can choose to
    assign a target object by either calling setTargetObject() or by passing a
    QObject pointer to QtAnimation's constructor, or you can assign an item as
    the target by calling setTargetItem(), or by passing an item pointer to
    QtAnimation's constructor.

    QtAnimation also declares the virtual currentValueChanged() function, which
    makes it suitable to use as a base class for custom animations that are
    based on QVariant or property interpolation.

    \sa QtAnimationGroup, QtAbstractAnimation, QtTransition, {The Animation Framework}
*/

/*!
    \fn void QtAnimation::valueChanged(const QVariant &value)

    QtAnimation emits this signal whenever the current \a value changes.

    \sa currentValue, startValue, targetValue
*/

/*!
    \fn void QtAnimation::durationChanged(int duration)

    QtAnimation emits this signal whenever the \a duration of the animation
    changes.

    \sa setDuration(), duration()
*/

#include "qanimation.h"
#include "qanimationgroup.h"
#include <QtCore/qdebug.h>
#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <QtGui/qgraphicswidget.h>

#include "qanimation_p.h"

#include <qmath.h>

QT_BEGIN_NAMESPACE


QHash<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> *QtAnimationPrivate::runningAnimations()
{
    static QHash<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> s;
    return &s;
}

static QVariant _q_interpolateVariant(const QVariant &from, const QVariant &to, qreal progress)
{
    QVariant res;
    switch (to.type()) {
    case QVariant::Int: {
        int f = from.toInt();
        int t = to.toInt();
        res = f + (t - f) * progress;
        break;
    }
    case QVariant::Double: {
        double f = from.toDouble();
        double t = to.toDouble();
        res = f + (t - f) * progress;
        break;
    }
    case QVariant::Rect: {
        QRect f = from.toRect();
        QRect t = to.toRect();
        res = QRect(_q_interpolateVariant(f.topLeft(), t.topLeft(), progress).toPoint(),
                    _q_interpolateVariant(f.size(), t.size(), progress).toSize());
        break;
    }
    case QVariant::Size: {
        QSize f = from.toSize();
        QSize t = to.toSize();
        res = QSize(_q_interpolateVariant(f.width(), t.width(), progress).toInt(),
                    _q_interpolateVariant(f.height(), t.height(), progress).toInt());
        break;
    }
    case QVariant::Point: {
        QPoint f = from.toPoint();
        QPoint t = to.toPoint();
        res = QPoint(_q_interpolateVariant(f.x(), t.x(), progress).toInt(),
                     _q_interpolateVariant(f.y(), t.y(), progress).toInt());
        break;
    }
    case QVariant::RectF: {
        QRectF f = from.toRectF();
        QRectF t = to.toRectF();
        res = QRectF(_q_interpolateVariant(f.topLeft(), t.topLeft(), progress).toPointF(),
                     _q_interpolateVariant(f.size(), t.size(), progress).toSizeF());
        break;
    }
    case QVariant::SizeF: {
        QSizeF f = from.toSizeF();
        QSizeF t = to.toSizeF();
        res = QSizeF(_q_interpolateVariant(f.width(), t.width(), progress).toDouble(),
                     _q_interpolateVariant(f.height(), t.height(), progress).toDouble());
        break;
    }
    case QVariant::PointF: {
        QPointF f = from.toPointF();
        QPointF t = to.toPointF();
        res = QPointF(_q_interpolateVariant(f.x(), t.x(), progress).toDouble(),
                      _q_interpolateVariant(f.y(), t.y(), progress).toDouble());
        break;
    }
    case QVariant::Bool: {
        if (to.toBool())
            progress == 1. ? res = true : res = false;
        else
            progress == 1. ? res = false : res = true;
        break;
    }
    case QVariant::Color: {
        QColor f = qVariantValue<QColor>(from);
        QColor t = qVariantValue<QColor>(to);

        res = QColor(_q_interpolateVariant(f.red(), t.red(), progress).toInt(),
            _q_interpolateVariant(f.green(), t.green(), progress).toInt(),
            _q_interpolateVariant(f.blue(), t.blue(), progress).toInt(),
            _q_interpolateVariant(f.alpha(), t.alpha(), progress).toInt());

        break;
    }
    default:
        res = to;
        break;
    }

    return res;
}

static void _q_QGraphicsItem_setProperty(QGraphicsItem *item, const QByteArray &propertyName, const QVariant &value)
{
    if (!qstrcmp(propertyName, "pos")) {
        item->setPos(value.toPointF());
    } else if (!qstrcmp(propertyName, "geometry")) {
        if (!item->isWidget())
            return;
        ((QGraphicsWidget *)item)->setGeometry(value.toRectF());
#if !defined(QT_EXPERIMENTAL_SOLUTION)
    } else if (!qstrcmp(propertyName, "opacity")) {
        item->setOpacity(value.toDouble());
#endif
    } else if (!qstrcmp(propertyName, "visible")) {
        item->setVisible(value.toBool());
#if !defined(QT_EXPERIMENTAL_SOLUTION)
    } else if (!qstrcmp(propertyName, "scaleFactorX")) {
        item->setScaleFactorX(value.toDouble());
    } else if (!qstrcmp(propertyName, "scaleFactorY")) {
        item->setScaleFactorY(value.toDouble());
    } else if (!qstrcmp(propertyName, "rotationAngleX")) {
        item->setRotationAngleX(value.toDouble());
    } else if (!qstrcmp(propertyName, "rotationAngleY")) {
        item->setRotationAngleY(value.toDouble());
    } else if (!qstrcmp(propertyName, "rotationAngleZ")) {
        item->setRotationAngleZ(value.toDouble());
#endif
    }
}

static QVariant _q_QGraphicsItem_property(QGraphicsItem *item, const QByteArray &propertyName)
{
    if (!qstrcmp(propertyName, "pos"))
        return item->pos();
    if (!qstrcmp(propertyName, "geometry")) {
        if (!item->isWidget())
            return QVariant();
        return ((QGraphicsWidget *)item)->geometry();
    }
#if !defined(QT_EXPERIMENTAL_SOLUTION)
    if (!qstrcmp(propertyName, "opacity"))
        return double(item->opacity());
#endif
    if (!qstrcmp(propertyName, "visible"))
        return item->isVisible();
    
#if !defined(QT_EXPERIMENTAL_SOLUTION)
    if (!qstrcmp(propertyName, "scaleFactorX"))
        return item->scaleFactorX();
    if (!qstrcmp(propertyName, "scaleFactorY"))
        return item->scaleFactorX();
    if (!qstrcmp(propertyName, "rotationAngleX"))
        return item->rotationAngleX();
    if (!qstrcmp(propertyName, "rotationAngleY"))
        return item->rotationAngleY();
    if (!qstrcmp(propertyName, "rotationAngleZ"))
        return item->rotationAngleZ();
#endif

    
    return QVariant();
}

void QtAnimationPrivate::updateCurrentValue()
{
    Q_Q(QtAnimation);
    currentValue = QVariant();

    if (lastValue != q->currentValue()) {
        lastValue = currentValue;
        q->updateCurrentValue(currentValue);
        emit q->valueChanged(currentValue);
    }
}

qreal QtAnimationPrivate::currentStep() const
{
    if (duration == 0)
        return qreal(0);
    return qBound<qreal>(qreal(0.0), (((currentTime - 1) % duration) + 1) / qreal(duration), qreal(1.0));
}

void QtAnimationPrivate::updateProperty()
{
    Q_Q(QtAnimation);
    if (!target || !q->isRunning())
        return;

    if (target.object() && hasMetaProperty == 0 && !property.isValid()) {
        const QMetaObject *mo = target.object()->metaObject();
        const int index = mo->indexOfProperty(propertyName);
        if (index != -1) {
            hasMetaProperty = 1;
            property = mo->property(index);
        } else {
            hasMetaProperty = 2;
        }
    }
    if (hasMetaProperty == 1) {
        property.write(target.object(), currentValue);
    } else {
        if (target.object())
            target.object()->setProperty(propertyName.constData(), currentValue);
        else
            _q_QGraphicsItem_setProperty(target.item(), propertyName.constData(), currentValue);
    }
}

void QtAnimationPrivate::_q_objectDeleted()
{
    Q_Q(QtAnimation);
    q->stop();
    q->deleteLater();
}

/*!
    Construct a QtAnimation object. \a parent is passed to QObject's
    constructor.
*/
QtAnimation::QtAnimation(QObject *parent)
    : QtAbstractAnimation(*new QtAnimationPrivate, parent)
{
}

/*!
    Construct a QtAnimation object. \a parent is passed to QObject's
    constructor. The animation changes the property \a propertyName on \a
    target. The default duration is 250ms.

    \sa targetObject, propertyName
*/
QtAnimation::QtAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QtAbstractAnimation(*new QtAnimationPrivate, parent)
{
    setTargetObject(target);
    setPropertyName(propertyName);
}

/*!
    Construct a QtAnimation object. \a parent is passed to QObject's
    constructor. The animation changes the property \a propertyName on \a
    target. The default duration is 250ms.

    \sa targetItem, propertyName
*/
QtAnimation::QtAnimation(QGraphicsItem *target, const QByteArray &propertyName, QObject *parent)
    : QtAbstractAnimation(*new QtAnimationPrivate, parent)
{
    setTargetItem(target);
    setPropertyName(propertyName);
}

/*!
    \internal

    For disambiguation.
*/
QtAnimation::QtAnimation(QGraphicsWidget *target, const QByteArray &propertyName, QObject *parent)
    : QtAbstractAnimation(*new QtAnimationPrivate, parent)
{
    setTargetItem((QGraphicsItem *)target);
    setPropertyName(propertyName);
}

/*!
    \internal
 */
QtAnimation::QtAnimation(QtAnimationPrivate &dd, QObject *parent)
    : QtAbstractAnimation(dd, parent)
{
}

/*!
    Destroys the QtAnimation instance.
 */
QtAnimation::~QtAnimation()
{
    if (isRunning()) {
        stop();
    }
}

/*!
    \property QtAnimation::easingCurve
    \brief the easing curve of the animation

    This property defines the easing curve of the animation. By default, a
    linear easing curve is used, resulting in linear interpolation of the
    target property. For many animations, it's useful to try different easing
    curves, including QtEasingCurve::InCirc, which provides a circular entry curve,
    and QtEasingCurve::InOutElastic, which provides an elastic effect on the values
    of the interpolated property.

    The easing curve is used with the interpolator, the interpolated() virtual
    function, the animation's duration, and loopCount, to control how the
    current value changes as the animation progresses.
*/
QtEasingCurve QtAnimation::easingCurve() const
{
    Q_D(const QtAnimation);
    return d->easing;
}
void QtAnimation::setEasingCurve(const QtEasingCurve &easing)
{
    Q_D(QtAnimation);
    d->easing = easing;
    d->updateCurrentValue();
}

/*!
    \property QtAnimation::duration
    \brief the duration of the animation

    This property describes the duration of the animation. The default
    duration is 250 milliseconds.

    \sa QtAbstractAnimation::duration()
 */
int QtAnimation::duration() const
{
    Q_D(const QtAnimation);
    return d->duration;
}
void QtAnimation::setDuration(int msecs)
{
    Q_D(QtAnimation);
    if (msecs < 0) {
        qWarning("QtAnimation::setDuration: cannot set a negative duration");
        return;
    }
    if (d->duration == msecs)
        return;
    d->duration = msecs;
    d->updateCurrentValue();
    emit durationChanged(msecs);
}

/*!
    \property QtAnimation::startValue
    \brief the optional start value of the animation

    This property describes the optional start value of the animation. If
    omitted, or if a null QVariant is assigned as the start value, the
    animation will use the current position of the target when the animation
    is started.

    \sa targetValue
*/
QVariant QtAnimation::startValue() const
{
    Q_D(const QtAnimation);
    return d->startValue;
}
void QtAnimation::setStartValue(const QVariant &value)
{
    Q_D(QtAnimation);
    if (d->startValue == value)
        return;
    d->hasStartValue = value.isValid();
    d->startValue = value;
    d->updateCurrentValue();
}

/*!
    \property QtAnimation::targetValue
    \brief the target value of the animation

    This property describes the target value of the animation.

    \sa startValue
 */
QVariant QtAnimation::targetValue() const
{
    Q_D(const QtAnimation);
    return d->targetValue;
}
void QtAnimation::setTargetValue(const QVariant &value)
{
    Q_D(QtAnimation);
    if (d->targetValue == value)
        return;
    d->hasTargetValue = value.isValid();
    d->targetValue = value;
    d->updateCurrentValue();
}

/*!
    \property QtAnimation::currentValue
    \brief the current value of the animation

    This property describes the current value; an interpolation between the
    start value and the end value, using the current time for progress.

    QtAnimation calls the virtual updateCurrentValue() function when the
    current value changes. This is particularily useful for subclasses that
    need to track updates.

    \sa startValue, targetValue
 */
QVariant QtAnimation::currentValue() const
{
    Q_D(const QtAnimation);
    if (!d->currentValue.isValid()) {
        QVariant start = d->hasStartValue ? d->startValue : d->defaultStartValue;
        QVariant target = d->hasTargetValue ? d->targetValue : d->defaultTargetValue;
        const_cast<QtAnimationPrivate *>(d)->currentValue
            = interpolated(start, target, d->easing.valueForStep(d->currentStep()));
    }
    return d->currentValue;
}

/*!
    \property QtAnimation::targetObject
    \brief the target QObject for this animation.

    This property defines the target QObject for this animation. If you want
    to animate properties for a QGraphicsItem instead, you can call
    setTargetItem(). If the animation has no target, or if the target is a
    QGraphicsItem, this property will be 0.

    \sa targetItem
 */
QObject *QtAnimation::targetObject() const
{
    Q_D(const QtAnimation);
    return d->target.object();
}
void QtAnimation::setTargetObject(QObject *target)
{
    Q_D(QtAnimation);
    if (d->target.object() == target)
        return;
    if (d->target.object())
        disconnect(d->target.object(), SIGNAL(destroyed()), this, SLOT(_q_objectDeleted()));

    d->target = target;
    d->hasMetaProperty = 0;
    d->updateProperty();
    if (target)
        connect(target, SIGNAL(destroyed()), this, SLOT(_q_objectDeleted()));
}

/*!
    \property QtAnimation::targetItem
    \brief the target QGraphicsItem for this animation.

    This property defines the target QGraphicsItem for this animation. If you
    want to animate properties for a QObject instead, you can call
    setTargetObject(). If the animation has no target, or if the target is a
    QObject, this property will be 0.

    \sa targetObject
 */
QGraphicsItem *QtAnimation::targetItem() const
{
    Q_D(const QtAnimation);
    return d->target.item();
}
void QtAnimation::setTargetItem(QGraphicsItem *target)
{
    Q_D(QtAnimation);
    if (d->target.item() == target)
        return;

    if (d->target.object())
        disconnect(d->target.object(), SIGNAL(destroyed()), this, SLOT(_q_objectDeleted()));

    d->target = target;
    d->hasMetaProperty = 0;
    d->updateProperty();
}

/*!
    \property QtAnimation::propertyName
    \brief the target property name for this animation

    This property defines the target property name for this animation. The
    property name is required for the animation to operate.
 */
QByteArray QtAnimation::propertyName() const
{
    Q_D(const QtAnimation);
    return d->propertyName;
}
void QtAnimation::setPropertyName(const QByteArray &propertyName)
{
    Q_D(QtAnimation);
    d->propertyName = propertyName;
    d->hasMetaProperty = 0;
    d->updateProperty();
}

void QtAnimation::setPropertyName(const QString &propertyName)
{
    setPropertyName(propertyName.toAscii());
}

/*!
    \brief convenience function to construct a QtAnimation that animates the "pos" property.

    This is a convenience function that you can use to animate the "pos"
    property of \a item. \a pos it the target position, \a duration is the desired duration and 
    with \a easing you can specify the easing curve you want to use.
    It also provides a type-safe way of specifying the
    property you want to animate. The animation returned can either be added to 
    a QtAnimationGroup or can be started immediately.
    
    Calling this convenience function with the following arguments:
    \code
        QtAnimation *anim = QtAnimation::posAnimation(item, QPointF(240, 0), 1000, QtEasingCurve::EaseInOut));
    \endcode
    is the same as this code:
    \code
        QtAnimation *anim = new QtAnimation(item, "pos");
        anim->setDuration(1000);
        anim->setEasingCurve(QtEasingCurve::EaseInOut);
        anim->setTargetValue(QPointF(240, 0));
    \endcode

    If you want to immediately start the animation and let the animation
    delete itself when it is finished you can do:
    \code
        QGraphicsItem *item;
        QtAnimation::posAnimation(item, QPointF(240, 0), 250, QtEasingCurve::EaseInOut)->start(QtAnimation::DeleteWhenFinished);
    \endcode
    
    \sa geometryAnimation(), opacityAnimation(), propertyAnimation(), visibleAnimation()
*/
QtAnimation* QtAnimation::posAnimation(QGraphicsItem *item, const QPointF &pos,
                             int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation(item, "pos");
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(pos);
    return animation;
}

/*!
    \overload
    This is a convenience function that you can use to construct a simple
    animation with a single line of code. Use this if you want to animate \a widget.

    \sa geometryAnimation(), opacityAnimation(), propertyAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::posAnimation(QWidget *widget, const QPoint &pos,
                             int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation((QObject *)widget, "pos");
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(pos);
    return animation;
}

/*!
    \brief convenience function to construct a QtAnimation that animates the "geometry" property.

    This is a convenience function that you can use to animate the "geometry"
    property of \a widget. \a rect it the target rectangle, \a duration 
    is the desired duration and with \a easing you can specify the easing 
    curve you want to use.
    It also provides a type-safe way of specifying the property you want to 
    animate.The animation returned can either be added to 
    a QtAnimationGroup or can be started immediately.
    
    \sa posAnimation(), opacityAnimation(), propertyAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::geometryAnimation(QGraphicsWidget *widget, const QRectF &rect,
                                  int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation((QObject *)widget, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(rect);
    return animation;
}

/*!
    \overload
    This is a convenience function that you can use to construct a simple
    animation with a single line of code. Use this if you want to animate a QWidget.

    \sa geometryAnimation(), opacityAnimation(), propertyAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::geometryAnimation(QWidget *widget, const QRect &rect,
                                  int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation((QObject *)widget, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(rect);
    return animation;
}

/*!
    \brief convenience function to construct a QtAnimation that animates the "opacity" property.

    This is a convenience function that you can use to animate the "opacity"
    property of \a item. \a opacity it the target opacity, \a duration 
    is the desired duration and with \a easing you can specify the easing 
    curve you want to use.
    It also provides a type-safe way of specifying the property you want to 
    animate.The animation returned can either be added to 
    a QtAnimationGroup or can be started immediately.

    \sa posAnimation(), geometryAnimation(), propertyAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::opacityAnimation(QGraphicsItem *item, qreal opacity,
                                 int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation(item, "opacity");
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(opacity);
    return animation;
}

/*!
    \brief convenience function to construct a QtAnimation that animates the property \a propertyName.

    This is a convenience function that you can use to animate the \a propertyName
    property of \a item. \a value it the target value, \a duration 
    is the desired duration and with \a easing you can specify the easing 
    curve you want to use.
    It also provides a type-safe way of specifying the property you want to 
    animate.The animation returned can either be added to 
    a QtAnimationGroup or can be started immediately.

    \sa posAnimation(), geometryAnimation(), opacityAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::propertyAnimation(QGraphicsItem *item, const char *propertyName, const QVariant &value,
                                  int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation(item, propertyName);
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(value);
    return animation;
}

/*!
    \overload
    This is a convenience function that you can use to construct a simple
    animation with a single line of code. Use this if you want to animate \a widget.

    \sa posAnimation(), geometryAnimation(), opacityAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::propertyAnimation(QGraphicsWidget *widget, const char *propertyName, const QVariant &value,
                                  int duration, const QtEasingCurve &easing)
{
    return propertyAnimation((QObject *)widget, propertyName, value, duration, easing);
}

/*!
    \overload
    This is a convenience function that you can use to construct a simple
    animation with a single line of code. Use this if you want to animate \a object.

    \sa posAnimation(), geometryAnimation(), opacityAnimation(), visibleAnimation()
 */
QtAnimation* QtAnimation::propertyAnimation(QObject *object, const char *propertyName, const QVariant &value,
                                         int duration, const QtEasingCurve &easing)
{
    QtAnimation *animation = new QtAnimation(object, propertyName);
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setTargetValue(value);
    return animation;
}

/*!
    \brief convenience function to construct a QtAnimation that animates the "visible" property.

    This is a convenience function that you can use to animate the "visible"
    property of \a item. \a visible it the target visibility, \a duration 
    is the desired duration and with \a easing you can specify the easing 
    curve you want to use.
    It also provides a type-safe way of specifying the property you want to 
    animate.The animation returned can either be added to 
    a QtAnimationGroup or can be started immediately.

    \sa posAnimation(), geometryAnimation(), opacityAnimation(), propertyAnimation()
 */
QtAnimation* QtAnimation::visibleAnimation(QGraphicsItem *item, bool visible)
{
    QtAnimation *animation = new QtAnimation(item, "visible");
    animation->setTargetValue(visible);
    return animation;
}

/*!
    \overload
    This is a convenience function that you can use to construct a simple
    animation with a single line of code. Use this if you want to animate a QWidget.

    \sa posAnimation(), geometryAnimation(), opacityAnimation(), propertyAnimation()
 */
QtAnimation* QtAnimation::visibleAnimation(QWidget *widget, bool visible)
{
    QtAnimation *animation = new QtAnimation((QObject*)widget, "visible");
    animation->setTargetValue(visible);
    return animation;
}

/*!
    \reimp
 */
bool QtAnimation::event(QEvent *event)
{
    return QtAbstractAnimation::event(event);
}

/*!
    This virtual function returns the linear interpolation between variants \a
    from and \a to, at \a progress, usually a value between 0 and 1. You can reimplement
    this function in a subclass of QtAnimation to provide your own interpolation
    algorithm. Note that in order for the interpolation to work with a QtEasingCurve
    that return a value smaller than 0 or larger than 1 (such as QtEasingCurve::InBack)
    you should make sure that it can extrapolate. If the semantic of the datatype
    does not allow extrapolation this function should handle that gracefully.
    
    \sa QtEasingCurve
 */
QVariant QtAnimation::interpolated(const QVariant &from, const QVariant &to, qreal progress) const
{
    return _q_interpolateVariant(from, to, progress);
}

/*!
    This virtual function is called by QtAnimation whenever the current value
    changes. \a value is the new, updated value. By reimplementing this
    function in a subclass of QtAnimation, you can easily apply the new value
    to a custom animation target.

    \sa currentValue, currentTime
 */
void QtAnimation::updateCurrentValue(const QVariant &value)
{
    Q_D(QtAnimation);
    Q_UNUSED(value);
    d->updateProperty();
}

/*!
    \reimp
 */
void QtAnimation::updateCurrentTime(int msecs)
{
    Q_D(QtAnimation);
    Q_UNUSED(msecs);
    d->updateCurrentValue();
}

/*!
    \reimp
*/
void QtAnimation::updateRunning(bool isRunning)
{
    Q_D(QtAnimation);

    QPair<QtAnimationPointer, QByteArray> key = qMakePair(d->target, d->propertyName);
    if(isRunning) {
        if (d->runningAnimations()->contains(key)) {
            QtAnimation *oldAnim = d->runningAnimations()->value(key);
            if(oldAnim != this) {
                oldAnim->stop();
            }
        }
        d->runningAnimations()->insert(key, this);
    } else {
        if (d->runningAnimations()->value(key) == this)
            d->runningAnimations()->remove(key);
    }

    // Initialize start value
    if (isRunning && !d->hasStartValue
        && ((d->currentTime == 0 && (d->currentLoop || d->currentLoop == 0))
            || (d->currentTime == duration() && d->currentLoop == (d->loopCount - 1)))) {
        if (d->target.object()) {
            d->defaultStartValue = d->target.object()->property(d->propertyName.constData());
        } else {
            d->defaultStartValue = _q_QGraphicsItem_property(d->target.item(), 
                                                             d->propertyName.constData());
        }
    }
}

#include "moc_qanimation.cpp"

QT_END_NAMESPACE

