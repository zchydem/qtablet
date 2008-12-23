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

#ifndef QANIMATION_H
#define QANIMATION_H

#include "qabstractanimation.h"
#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qeasingcurve.h"
#else
# include <QtCore/qeasingcurve.h>
#endif
#include <QtCore/qbasictimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/qpoint.h>
#include <QtCore/qtimeline.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QGraphicsItemAnimator;
class QGraphicsWidget;

class QtAbstractAnimationPrivate;
class QAbstractAnimator;
class QtAnimationGroup;
class QtAnimationPrivate;

class QtAnimationGroupPrivate;
class Q_ANIMATION_EXPORT QtAnimation : public QtAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName) /*NOTIFY propertyNameChanged*/
    Q_PROPERTY(QVariant startValue READ startValue WRITE setStartValue) /*NOTIFY startValueChanged*/
    Q_PROPERTY(QVariant targetValue READ targetValue WRITE setTargetValue) /*NOTIFY targetValueChanged*/
    Q_PROPERTY(QVariant currentValue READ currentValue NOTIFY valueChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject) /*NOTIFY targetObjectChanged*/
    Q_PROPERTY(QGraphicsItem* targetItem READ targetItem WRITE setTargetItem)  /*NOTIFY targetItemChanged*/
    Q_PROPERTY(QtEasingCurve easingCurve READ easingCurve WRITE setEasingCurve) /*NOTIFY easingCurveChanged*/

public:
    QtAnimation(QObject *parent = 0);
    QtAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);
    QtAnimation(QGraphicsItem *target, const QByteArray &propertyName, QObject *parent = 0);
    QtAnimation(QGraphicsWidget *target, const QByteArray &propertyName, QObject *parent = 0);
    ~QtAnimation();

    QtEasingCurve easingCurve() const;
    void setEasingCurve(const QtEasingCurve &easing);

    int duration() const;
    void setDuration(int msecs);

    QVariant startValue() const;
    void setStartValue(const QVariant &value);

    QVariant targetValue() const;
    void setTargetValue(const QVariant &value);

    QVariant currentValue() const;

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QGraphicsItem *targetItem() const;
    void setTargetItem(QGraphicsItem *target);

    QByteArray propertyName() const;
    void setPropertyName(const QByteArray &propertyName);

    static QtAnimation* posAnimation(QGraphicsItem *item, const QPointF &pos,
                              int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* posAnimation(QWidget *widget, const QPoint &pos,
                              int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* geometryAnimation(QGraphicsWidget *widget, const QRectF &rect,
                                  int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* geometryAnimation(QWidget *widget, const QRect &rect,
                                  int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* opacityAnimation(QGraphicsItem *item, qreal opacity,
                                 int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* propertyAnimation(QGraphicsItem *item, const char *propertyName,
           const QVariant &value, int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* propertyAnimation(QGraphicsWidget *widget, const char *propertyName,
           const QVariant &value, int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* propertyAnimation(QObject *object, const char *propertyName,
          const QVariant &value, int duration = 250, const QtEasingCurve &easing = QtEasingCurve());

    static QtAnimation* visibleAnimation(QGraphicsItem *item, bool visible);
    static QtAnimation* visibleAnimation(QWidget *widget, bool visible);


Q_SIGNALS:
    void valueChanged(const QVariant &value);
    void durationChanged(int msecs);

protected:
    QtAnimation(QtAnimationPrivate &dd, QObject *parent = 0);
    bool event(QEvent *event);

    virtual QVariant interpolated(const QVariant &from, const QVariant &to, qreal progress) const;
    virtual void updateCurrentValue(const QVariant &value);
    void updateCurrentTime(int msecs);
    void updateRunning(bool isRunning);
private:
    Q_DISABLE_COPY(QtAnimation)
    Q_DECLARE_PRIVATE(QtAnimation)
    Q_PRIVATE_SLOT(d_func(), void _q_objectDeleted())

    void setPropertyName(const QString &propertyName);
        
    friend class QAbstractAnimator;
    friend class QAbstractAnimatorPrivate;
    friend class QtState;
    friend class QtStatePrivate;
    friend class QtTransition;
    friend class QtTransitionPrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QANIMATION_H
