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

#include "qpropertystate_p.h"

#include <QtCore/qvariant.h>
#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "common.h"
#endif

#ifdef QT_EXPERIMENTAL_SOLUTION
class QPropertyStatePrivate
#else
class QPropertyStatePrivate : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QPropertyState);
public:
    QPropertyStatePrivate() : name(0) { qRegisterMetaType<QtAnimationPointer>(); }
    QtAnimationPointer target;
    QByteArray name;
    QVariant value;
#ifdef QT_EXPERIMENTAL_SOLUTION
    QObject *q_ptr;
#endif
};

#ifdef QT_EXPERIMENTAL_SOLUTION
QPropertyState::QPropertyState(QObject *parent)
    : d_ptr(new QPropertyStatePrivate)
{
    setParent(parent);
}
#else
QPropertyState::QPropertyState(QObject *parent)
    : QObject(*(new QPropertyStatePrivate), parent)
{
    d_ptr->q_ptr = this;
}
#endif

#ifdef QT_EXPERIMENTAL_SOLUTION
QPropertyState::QPropertyState(QtAnimationPointer target, const QByteArray &propertyName,
                               const QVariant &value, QObject *parent)
  : d_ptr(new QPropertyStatePrivate)
{
    setTarget(target);
    setPropertyName(propertyName);
    setValue(value);
    setParent(parent);
    d_ptr->q_ptr = this;
}
#else
QPropertyState::QPropertyState(QtAnimationPointer target, const QByteArray &propertyName,
                               const QVariant &value, QObject *parent)
  : QObject(*(new QPropertyStatePrivate), parent)
{
    setTarget(target);
    setPropertyName(propertyName);
    setValue(value);
}
#endif

QPropertyState::~QPropertyState()
{
}

void QPropertyState::setTarget(const QtAnimationPointer &target)
{
    Q_D(QPropertyState);
    d->target = target;
}

QtAnimationPointer QPropertyState::target() const
{
    Q_D(const QPropertyState);
    return d->target;
}

void QPropertyState::setTargetObject(QObject *target)
{
    Q_D(QPropertyState);
    d->target = target;
}

QObject *QPropertyState::targetObject() const
{
    Q_D(const QPropertyState);
    return d->target;
}
    
void QPropertyState::setTargetItem(QGraphicsItem *target)
{
    Q_D(QPropertyState);
    d->target = target;
}

QGraphicsItem *QPropertyState::targetItem() const
{
    Q_D(const QPropertyState);
    return d->target;
}

void QPropertyState::setPropertyName(const QByteArray &name)
{
    Q_D(QPropertyState);
    d->name = name;
}

QByteArray QPropertyState::propertyName() const
{
    Q_D(const QPropertyState);
    return d->name;
}

void QPropertyState::setPropertyName(const QString &name)
{
    Q_D(QPropertyState);
    d->name = name.toAscii();
}

void QPropertyState::setValue(const QVariant &value)
{
    Q_D(QPropertyState);
    QVariant oldValue = d->value;
    d->value = value;
    emit valueChanged(value, oldValue);
}

QVariant QPropertyState::value() const
{
    Q_D(const QPropertyState);
    return d->value;
}

QT_END_NAMESPACE

#include "moc_qpropertystate_p.cpp"
