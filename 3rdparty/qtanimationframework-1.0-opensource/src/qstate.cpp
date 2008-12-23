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
    \class QtState
    \brief The QtState class describes a state with connections and properties
    for QObjects, QWidgets and QGraphicsItems.
    \since 4.5
    \ingroup animation
    \preliminary

    This class is part of {The Animation Framework}.

    QtState provides a convenient API for configuring properties, connections
    and other features related to state changes in your application. When used
    together with QtTransition, it also allows you to define how these values
    change, possibly using animations over time.

    States can be independent, or part of a group of mutually exclusive states
    (i.e., only one state in a group can be active at any time). You can
    activate or deactivate a state by calling the setActive(), activate() or
    deactivate() slots. When the state is activated, it configures all
    properties and connections for that state, and it also calls the virtual
    enterState() and leaveState() functions, which you can reimplement in a
    subclass of QtState to provide custom state change logic.  QtState also
    emits the activated() and deactivated() signals, so that you can define
    custom state logic in a connected slot.

    \sa QtTransition, QtAnimation, {The Animation Framework}
*/

/*!
    \fn void QtState::activated(bool active)

    QtState emits this signal when the state is either activated or
    deactivated.  If \a active is true, the state is activated; otherwise, it
    is deactivated.

    \sa deactivated(), setActive()
*/

/*!
    \fn void QtState::activated()

    QtState emits this signal when the state is activated.

    \sa deactivated(), setActive()
*/

/*!
    \fn void QtState::deactivated()

    QtState emits this signal when the state is deactivated.

    \sa activated(), setActive()
*/

#include "qstate.h"
#include "qanimationgroup_p.h"
#include "qstate_p.h"

#include "qanimation.h"
#include "qstategroup.h"
#include "qtransition.h"
#include <QtCore/qdebug.h>
#include <QtCore/qset.h>
#include <QtGui/qgraphicswidget.h>

QT_BEGIN_NAMESPACE


QSet<QtTransition *> QtStatePrivate::anyTransitions;

static void _q_QGraphicsItem_setProperty(QGraphicsItem *item, const char *propertyName, const QVariant &value)
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
    }
}

static QVariant _q_QGraphicsItem_property(QGraphicsItem *item, const char *propertyName)
{
    // ### extend with all "properties" of QGraphicsItem
    if (!strcmp(propertyName, "pos")) {
        return item->pos();
    } else if (!qstrcmp(propertyName, "geometry")) {
        if (item->isWidget())
            return ((QGraphicsWidget *)item)->geometry();
#if !defined(QT_EXPERIMENTAL_SOLUTION)
    } else if (!strcmp(propertyName, "opacity")) {
        return item->opacity();
#endif
    }
    return QVariant();
}

void QtStatePrivate::initializeStartPropertyHelper(QtAbstractAnimation *animation,
                                                  QMultiMap<QtAnimationPointer, QByteArray> *data)
{
    if (QtAnimation *anim = qobject_cast<QtAnimation *>(animation)) {
        data->insert(anim->d_func()->target, anim->propertyName());
    } else if (QtAnimationGroup *anim = qobject_cast<QtAnimationGroup *>(animation)) {
        QList<QtAbstractAnimation *> *anims = &anim->d_func()->animations;
        for (int i = 0; i < anims->size(); ++i)
            initializeStartPropertyHelper(anims->at(i), data);
    }
}

void QtStatePrivate::initializeTargetPropertyHelper(QtAbstractAnimation *animation,
                                                   QMap<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> *data)
{
    if (QtAnimation *anim = qobject_cast<QtAnimation *>(animation)) {
        QtAnimationPointer ptr;
        if (anim->targetObject())
            ptr = anim->targetObject();
        else if (anim->targetItem())
            ptr = anim->targetItem();
        data->insert(qMakePair<QtAnimationPointer, QByteArray>(ptr, anim->propertyName()), anim);
    } else if (QtAnimationGroup *anim = qobject_cast<QtAnimationGroup *>(animation)) {
        QList<QtAbstractAnimation *> *anims = &anim->d_func()->animations;
        for (int i = 0; i < anims->size(); ++i)
            initializeTargetPropertyHelper(anims->at(i), data);
    }
}

void QtStatePrivate::initializeTransition(QtTransition *transition, QtState *fromState, QtState *toState)
{
    if (!fromState && !toState) {
        // Cannot happen
        return;
    }

    if (fromState) {
        QMultiMap<QtAnimationPointer, QByteArray> data;
        QtStatePrivate::initializeStartPropertyHelper(transition, &data);
        QMapIterator<QtAnimationPointer, QByteArray> it(data);
        while (it.hasNext()) {
            it.next();
            QVariant propertyValue;
            if (QObject *o = it.key().object()) {
                propertyValue = fromState->property(it.key().object(), it.value());
                o->setProperty(it.value(), propertyValue);
            } else {
                propertyValue = fromState->property(it.key().item(), it.value());
                _q_QGraphicsItem_setProperty(it.key().item(), it.value(), propertyValue);
            }
        }
    }

    if (toState) {
        QMap<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> data;
        QtStatePrivate::initializeTargetPropertyHelper(transition, &data);
        QMapIterator<QPair<QtAnimationPointer, QByteArray>, QtAnimation *> it(data);
        while (it.hasNext()) {
            it.next();
            QtAnimationPointer ptr = it.key().first;
            QtAnimation *anim = it.value();
            QVariant propertyValue;
            if (ptr.object())
                propertyValue = toState->property(ptr.object(), it.key().second);
            else
                propertyValue = toState->property(ptr.item(), it.key().second);
            anim->d_func()->defaultTargetValue = propertyValue;
        }
    }
}

/*!
    \internal
 */
void QtStatePrivate::dig(QtAnimationGroup *group, QSet<QPair<QtAnimationPointer, QByteArray> > *props) const
{
    for (int i = 0; i < group->d_func()->animations.size(); ++i) {
        QtAbstractAnimation *a = group->d_func()->animations.at(i);
        if (QtAnimation *p = qobject_cast<QtAnimation *>(a)) {
            *props << qMakePair<QtAnimationPointer, QByteArray>(p->d_func()->target, p->propertyName());
        } else if (QtAnimationGroup *g = qobject_cast<QtAnimationGroup *>(a)) {
            dig(g, props);
        }
    }
}

/*!
    \internal
*/
void QtStatePrivate::clear_helper()
{
    qDeleteAll(oldPropertyStates);
    oldPropertyStates.clear();
    qDeleteAll(propertyStates);
    propertyStates.clear();
    qDeleteAll(connectionStates);
    connectionStates.clear();
    qDeleteAll(oldPropertyStates);
    oldPropertyStates.clear();
}
/*!
    Constructs a QtState object. \a parent is passed to QObject's constructor.
*/
#ifdef QT_EXPERIMENTAL_SOLUTION
QtState::QtState(QObject *parent)
    : d_ptr(new QtStatePrivate)
{
    // Let QtStateGroup qobject_cast to QtState in its
    // ChildAdded handler.
    setParent(parent);
    qRegisterMetaType<QtState *>();
    d_ptr->q_ptr = this;
}
#else
QtState::QtState(QObject *parent)
    : QObject(*new QtStatePrivate, 0)    //### parent
{
    // Let QtStateGroup qobject_cast to QtState in its
    // ChildAdded handler.
    setParent(parent);
    qRegisterMetaType<QtState *>();
}
#endif
/*!
    Destroys the QtState object.
*/
QtState::~QtState()
{
    Q_D(QtState);
    d->clear_helper();
}

/*!
    Deactivates and clears the state, removing all connections and property
    values for all objects and items. This function has no effect on the
    state's group membership, its active state and any defined transitions.

    If the state is active and has defined property values, these properties
    will also be unset in the target objects and items.

    \sa deactivate()
*/
void QtState::clear()
{
    Q_D(QtState);
    deactivate();
    d->clear_helper();
}

/*!
    \property QtState::group
    \brief the state's group

    This property describes the QtStateGroup that the state belongs to. If the
    state is not part of a group (i.e., an independent state), the group()
    function will return 0.
*/
QtStateGroup *QtState::group() const
{
    Q_D(const QtState);
    return d->group;
}
void QtState::setGroup(QtStateGroup *group)
{
    Q_D(QtState);
    if (d->group == group)
        return;
    if (d->group)
        d->group->removeState(this);
    if (group)
        group->addState(this);
}

/*!
    \property QtState::active
    \brief whether or not the state is active

    This property describes whether this state is active or not. You can
    activate a state by passing true to setActive(), or by calling
    activate(). A state can also become active if it's the default state of a
    QtStateGroup, and another state in the group is deactivated.

    To deactivate a group, you can either pass false to setActive(), or call
    deactivate().

    When a state is activated, it emits the activated() signal, and calls the
    virtual enterState() function. When it loses activation, it emits the
    deactivated() signal, and calls the virtual leaveState() function. The
    default implementations of enterState() and leaveState() manage the
    properties and connections defined for this state.

    \sa activate(), deactivate()
*/
bool QtState::isActive() const
{
    Q_D(const QtState);
    return d->active;
}
void QtState::setActive(bool active)
{
    Q_D(QtState);
    if (active == d->active)
        return;
    if (QtStateGroup *g = group()) {
        g->setActiveState(active ? this : 0);
        return;
    }
    if ((d->active = active)) {
        enterState(0);
        emit activated();
        emit activated(true);
    }  else {
        leaveState(0);
        emit deactivated();
        emit activated(false);
    }
}

/*!
    This function is equivalent to calling setActive(true).

    \sa deactivate(), active
*/
void QtState::activate()
{
    setActive(true);
}

/*!
    This function is equivalent to calling setActive(false).

    \sa activate(), active
*/
void QtState::deactivate()
{
    setActive(false);
}

/*!
    \property QtState::restoreProperties
    \brief whether the state should restore previous property values when deactivated

    If restoreProperties is true, the state will store the current value of
    all properties it changes as the state is activated, and restore these
    values when the state is deactivated again. By default this value is true.
*/
bool QtState::restoreProperties() const
{
    Q_D(const QtState);
    return d->restoreProperties;
}
void QtState::setRestoreProperties(bool enabled)
{
    Q_D(QtState);
    d->restoreProperties = enabled;
}

#ifndef QT_NO_PROPERTIES
/*!
    Instructs the state to set the property \a propertyName to \a value on \a
    object when activated.

    \sa property(), unsetProperty()
*/
void QtState::setProperty(QObject *object, const char *propertyName, const QVariant &value)
{
    Q_D(QtState);
    if (!object) {
        qWarning("QtState::setProperty: null object passed");
        return;
    }
    QPropertyState *state = new QPropertyState(object, propertyName, value, this);
    d->propertyStates << state;
}

/*!
    Returns the value that the property \a propertyName should have for \a
    object when this state is active.

    \sa setProperty(), unsetProperty()
*/
QVariant QtState::property(QObject *object, const char *propertyName) const
{
    Q_D(const QtState);
    if (!object) {
        qWarning("QtState::property: null object passed");
        return QVariant();
    }
    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *p = d->propertyStates.at(i);
        if (p->target().object() == object && qstrcmp(p->propertyName(), propertyName) == 0)
            return p->value();
    }
    return QVariant();
}

/*!
    Removes the value of the property \a propertyName for \a object for this
    state. When activated, this state will no longer affect the property.

    \sa setProperty(), property()
*/
void QtState::unsetProperty(QObject *object, const char *propertyName)
{
    Q_D(QtState);
    if (!object) {
        qWarning("QtState::unsetProperty: null object passed");
        return;
    }
    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *p = d->propertyStates.at(i);
        if (p->target().object() == object && qstrcmp(p->propertyName(), propertyName) == 0) {
            d->propertyStates.removeAt(i);
            break;
        }
    }
}

/*!
    \overload
    Instructs the state to set the property \a propertyName to \a value on \a
    item when activated.

    Although QGraphicsItem doesn't technically support properties like QObject
    does, this function emulates the behavior for all property-like functions
    in QGraphicsItem, allowing you as a user to treat the two the same way.

    \sa property(), unsetProperty()
*/
void QtState::setProperty(QGraphicsItem *item, const char *propertyName, const QVariant &value)
{
    Q_D(QtState);
    if (!item) {
        qWarning("QtState::setProperty: null item passed");
        return;
    }
    QPropertyState *state = new QPropertyState(item, propertyName, value, this);
    d->propertyStates << state;
}

/*!
    \overload
    Returns the value that the property \a propertyName should have for \a
    item when this state is active.

    \sa setProperty(), unsetProperty()
*/
QVariant QtState::property(QGraphicsItem *item, const char *propertyName) const
{
    Q_D(const QtState);
    if (!item) {
        qWarning("QtState::property: null item passed");
        return QVariant();
    }
    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *p = d->propertyStates.at(i);
        if (p->target().item() == item && qstrcmp(p->propertyName(), propertyName) == 0)
            return p->value();
    }
    return QVariant();
}

/*!
    \overload
    Removes the value of the property \a propertyName for \a item for this
    state. When activated, this state will no longer affect the property.

    \sa setProperty(), property()
*/
void QtState::unsetProperty(QGraphicsItem *item, const char *propertyName)
{
    Q_D(QtState);
    if (!item) {
        qWarning("QtState::unsetProperty: null item passed");
        return;
    }

    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *p = d->propertyStates.at(i);
        if (p->target().item() == item && qstrcmp(p->propertyName(), propertyName) == 0) {
            d->propertyStates.removeAt(i);
            break;
        }
    }
}

/*!
    \internal
    Only for disambiguation of QObject vs. QGraphicsItem when a QGraphicsWidget is passed.
*/
void QtState::setProperty(QGraphicsWidget *item, const char *propertyName, const QVariant &value)
{
    setProperty((QGraphicsItem *)item, propertyName, value);
}

/*!
    \internal
    Only for disambiguation of QObject vs. QGraphicsItem when a QGraphicsWidget is passed.
*/
QVariant QtState::property(QGraphicsWidget *item, const char *propertyName) const
{
    return property((QGraphicsItem *)item, propertyName);
}

/*!
    \internal
    Only for disambiguation of QObject vs. QGraphicsItem when a QGraphicsWidget is passed.
*/
void QtState::unsetProperty(QGraphicsWidget *item, const char *propertyName)
{
    unsetProperty((QGraphicsItem *)item, propertyName);
}
#endif

/*!
    Returns the position for \a item when this state is active.

    \sa setPos()
*/
QPointF QtState::pos(QGraphicsItem *item) const
{
    if (!item) {
        qWarning("QtState::pos: null item passed");
        return QPointF();
    }
    return property(item, "pos").toPointF();
}

/*!
    Instructs the state to set the position of \a item to \a pos when
    activated.

    \sa pos()
*/
void QtState::setPos(QGraphicsItem *item, const QPointF &pos)
{
    if (!item) {
        qWarning("QtState::setPos: null item passed");
        return;
    }
    setProperty(item, "pos", pos);
}

/*!
    Returns the position for \a widget when this state is active.

    \sa setPos()
*/
QPoint QtState::pos(QWidget *widget) const
{
    if (!widget) {
        qWarning("QtState::pos: null widget passed");
        return QPoint();
    }
    return property((QObject *)widget, "pos").toPoint();
}

/*!
    Instructs the state to set the position of \a widget to \a pos when
    activated.

    \sa pos()
*/
void QtState::setPos(QWidget *widget, const QPoint &pos)
{
    if (!widget) {
        qWarning("QtState::setPos: null widget passed");
        return;
    }
    setProperty((QObject *)widget, "pos", pos);
}

/*!
    Returns the geometry for \a widget when this state is active.

    \sa setGeometry()
*/
QRectF QtState::geometry(QGraphicsWidget *widget) const
{
    if (!widget) {
        qWarning("QtState::geometry: null widget passed");
        return QRectF();
    }
    return property((QObject *)widget, "geometry").toRectF();
}

/*!
    Instructs the state to set the geometry of \a widget to \a geom when
    activated.

    \sa geometry()
*/
void QtState::setGeometry(QGraphicsWidget *widget, const QRectF &geom)
{
    if (!widget) {
        qWarning("QtState::setGeometry: null widget passed");
        return;
    }
    setProperty((QGraphicsItem *)widget, "geometry", geom);
}

/*!
    Returns the geometry for \a widget when this state is active.

    \sa setGeometry()
*/
QRect QtState::geometry(QWidget *widget) const
{
    if (!widget) {
        qWarning("QtState::geometry: null widget passed");
        return QRect();
    }
    return property((QObject *)widget, "geometry").toRect();
}

/*!
    Instructs the state to set the geometry of \a widget to \a geom when
    activated.

    \sa geometry()
*/
void QtState::setGeometry(QWidget *widget, const QRect &geom)
{
    if (!widget) {
        qWarning("QtState::setGeometry: null widget passed");
        return;
    }
    setProperty((QObject *)widget, "geometry", geom);
}

/*!
    Returns the opacity for \a item when this state is active.

    \sa setOpacity()
*/
qreal QtState::opacity(QGraphicsItem *item) const
{
    if (!item) {
        qWarning("QtState::opacity: null item passed");
        return qreal(1.0);
    }
    return qreal(property(item, "opacity").toDouble());
}

/*!
    Instructs the state to set the opacity of \a item to \a opacity when this
    state is activated.

    \sa opacity()
*/
void QtState::setOpacity(QGraphicsItem *item, qreal opacity)
{
    if (!item) {
        qWarning("QtState::setOpacity: null item passed");
        return;
    }
    setProperty(item, "opacity", opacity);
}

/*!
    Instructs the state to establish a connection from \a sender and \a signal
    to \a receiver and \a member, using \a connectionType, when this state is
    activated.

    \sa unsetConnectionState()
*/
void QtState::setConnectionState(QObject *sender, const char *signal,
                                QObject *receiver, const char *member,
                                Qt::ConnectionType connectionType)
{
    Q_D(QtState);
    QConnectionState *state = new QConnectionState(sender, signal, receiver, member, connectionType);
    d->connectionStates << state;
}

/*!
    Removes the connection from \a sender and \a signal to \a receiver and \a
    member for this state.

    \sa setConnectionState()
*/
void QtState::unsetConnectionState(QObject *sender, const char *signal,
                          QObject *receiver, const char *member)
{
    Q_D(QtState);
    for (int i = 0; i < d->connectionStates.size(); ++i) {
        QConnectionState *state = d->connectionStates.at(i);
        if (state->sender() == sender && qstrcmp(state->signal(), signal) == 0
            && state->receiver() == receiver && qstrcmp(state->member(), member) == 0) {
            d->connectionStates.removeAt(i);
            break;
        }
    }
}

/*!
    \reimp
*/
bool QtState::event(QEvent *event)
{
    return QObject::event(event);
}

/*!
    This virtual function is called when this state is activated. \a fromState
    is the previous state. If there was no previous state, \a fromState is 0.

    The default implementation manages the properties and connections for this
    state.

    \sa leaveState()
*/
void QtState::enterState(QtState *fromState)
{
    Q_D(QtState);
    // ### Review this code, it's ugly and too slow

    if (d->restoreProperties && fromState) {
        QMapIterator<QPair<QtAnimationPointer, QByteArray>, QPropertyState *> it(fromState->d_func()->oldPropertyStates);
        while (it.hasNext()) {
            it.next();
            if (!d->oldPropertyStates.contains(it.key())) {
                QPropertyState *p = it.value();
                // ### Don't clone it, instead just refcount it
                d->oldPropertyStates[it.key()] = new QPropertyState(p->target(),
                                                                    p->propertyName(),
                                                                    p->value());
            }
        }
    }

    // Find all properties defined for this and the last state ### cache this
    QSet<QPair<QtAnimationPointer, QByteArray> > enterStateProperties;
    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *state = d->propertyStates.at(i);
        enterStateProperties << qMakePair<QtAnimationPointer, QByteArray>(state->target(), state->propertyName());
    }
    QSet<QPair<QtAnimationPointer, QByteArray> > leaveStateProperties;
    if (fromState) {
        QList<QPropertyState *> *leavePropertyStates = &fromState->d_func()->propertyStates;
        for (int i = 0; i < leavePropertyStates->size(); ++i) {
            QPropertyState *state = leavePropertyStates->at(i);
            leaveStateProperties << qMakePair<QtAnimationPointer, QByteArray>(state->target(), state->propertyName());
        }
    }

    // Priority 1: Transitions defined from and to these two states.
    // Priority 2: Transitions defined only from
    // Priority 3: Transitions defined only to
    // Priority 4: Any-to-any transitions

    QSet<QtTransition *> allTransitions = d->anyTransitions | d->enterTransitions;
    if (fromState)
        allTransitions |= fromState->d_func()->leaveTransitions;

    QSet<QPair<QtAnimationPointer, QByteArray> > propertiesWithTransitions;
    QSetIterator<QtTransition *> it(allTransitions);
    while (it.hasNext()) {
        QtTransition *t = it.next();
        if ((!t->fromState() || t->fromState() == fromState)
            && (!t->toState() || t->toState() == this)) {
            d->dig(t, &propertiesWithTransitions);
            d->initializeTransition(t, fromState, this);
            t->start();
        }
    }

    // Set all remaining properties / that have no transition.
    for (int i = 0; i < d->propertyStates.size(); ++i) {
        QPropertyState *state = d->propertyStates.at(i);
        if (!propertiesWithTransitions.contains(qMakePair<QtAnimationPointer, QByteArray>(state->target(), state->propertyName()))) {
            if (QObject *o = state->target().object()) {
                if (d->restoreProperties) {
                    QPair<QtAnimationPointer, QByteArray> pair(o, state->propertyName());
                    if (!d->oldPropertyStates.contains(pair)) {
                        d->oldPropertyStates[pair] = new QPropertyState(o, state->propertyName(), 
                                                                        o->property(state->propertyName()));
                    }
                }
                o->setProperty(state->propertyName(), state->value());
            } else if (QGraphicsItem *i = state->target().item()) {
                if (d->restoreProperties) {
                    QPair<QtAnimationPointer, QByteArray> pair(i, state->propertyName());
                    if (!d->oldPropertyStates.contains(pair)) {
                        d->oldPropertyStates[pair] = new QPropertyState(i, state->propertyName(), 
                                                                        _q_QGraphicsItem_property(i, state->propertyName()));
                    }
                }
                _q_QGraphicsItem_setProperty(i, state->propertyName(), state->value());
            }
        }
    }
    for (int i = 0; i < d->connectionStates.size(); ++i) {
        QConnectionState *state = d->connectionStates.at(i);
        connect(state->sender(), state->signal(), state->receiver(), state->member(),
                state->connectionType());
    }
}

/*!
    This virtual function is called by QtState when the state is
    deactivated. \a toState is a pointer to the next state. If there is no
    next state, \a toState is 0.

    The default implementation manages the properties and connections for this
    state.

    \sa enterState()
*/
void QtState::leaveState(QtState *toState)
{
    Q_D(QtState);
    // ### Review this code, it's ugly and too slow

    for (int i = 0; i < d->connectionStates.size(); ++i) {
        QConnectionState *state = d->connectionStates.at(i);
        disconnect(state->sender(), state->signal(), state->receiver(), state->member());
    }
    if (d->restoreProperties) {
        QSet<QPair<QtAnimationPointer, QByteArray> > definedPropertyStates;
        if (toState) {
            QList<QPropertyState *> *toStateProperties = &toState->d_func()->propertyStates;
            for (int i = 0; i < toStateProperties->size(); ++i) {
                QPropertyState *state = toStateProperties->at(i);
                definedPropertyStates << qMakePair<QtAnimationPointer, QByteArray>(state->target(), state->propertyName());
            }
        }
        QMapIterator<QPair<QtAnimationPointer, QByteArray>, QPropertyState *> it(d->oldPropertyStates);
        while (it.hasNext()) {
            it.next();
            const QPair<QtAnimationPointer, QByteArray> &pair = it.key();
            if (!definedPropertyStates.contains(pair)) {
                QPropertyState *state = it.value();
                if (QObject *o = state->target().object()) {
                    o->setProperty(state->propertyName(), state->value());
                } else if (QGraphicsItem *i = state->target().item()) {
                    _q_QGraphicsItem_setProperty(i, state->propertyName(), state->value());
                }
            }
        }
        if (!toState) {
            qDeleteAll(d->oldPropertyStates);
            d->oldPropertyStates.clear();
            if (QtStateGroup *g = group()) {
                foreach (QtState *state, g->states()) {
                    qDeleteAll(state->d_func()->oldPropertyStates);
                    state->d_func()->oldPropertyStates.clear();
                }
            }
        }
    }
}

#include "moc_qstate.cpp"

QT_END_NAMESPACE
