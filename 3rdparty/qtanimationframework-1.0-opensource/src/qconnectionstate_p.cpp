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

#include "qconnectionstate_p.h"
#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif

QT_BEGIN_NAMESPACE

#ifdef QT_EXPERIMENTAL_SOLUTION
class QConnectionStatePrivate
#else
class QConnectionStatePrivate : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QConnectionState);
public:
    QConnectionStatePrivate()
        : sender(0), signal(0),
          receiver(0), member(0),
          type(Qt::AutoConnection) {}
    QObject *sender;
    const char *signal;
    QObject *receiver;
    const char *member;
    Qt::ConnectionType type;
#ifdef QT_EXPERIMENTAL_SOLUTION
    QConnectionState *q_ptr;
#endif
};

#ifdef QT_EXPERIMENTAL_SOLUTION
QConnectionState::QConnectionState(QObject *sender, const char *signal,
                                   QObject *receiver, const char *member,
                                   Qt::ConnectionType type, QObject *parent)
    : d_ptr(new QConnectionStatePrivate)
{
    Q_D(QConnectionState);
    d->sender = sender;
    d->signal = signal;
    d->receiver = receiver;
    d->member = member;
    d->type = type;
    d_ptr->q_ptr = this;
    setParent(parent);
}

QConnectionState::QConnectionState(QObject *parent)
    : d_ptr(new QConnectionStatePrivate)
{
    d_ptr->q_ptr = this;
    setParent(parent);
}
#else
QConnectionState::QConnectionState(QObject *parent)
    : QObject(*new QConnectionStatePrivate, parent)
{
}

QConnectionState::QConnectionState(QObject *sender, const char *signal,
                                   QObject *receiver, const char *member,
                                   Qt::ConnectionType type, QObject *parent)
    : QObject(*new QConnectionStatePrivate, parent)
{
    Q_D(QConnectionState);
    d->sender = sender;
    d->signal = signal;
    d->receiver = receiver;
    d->member = member;
    d->type = type;
}
#endif

void QConnectionState::setSender(QObject *sender)
{
    Q_D(QConnectionState);
    d->sender = sender;
}

QObject *QConnectionState::sender() const
{
    Q_D(const QConnectionState);
    return d->sender;
}

void QConnectionState::setSignal(const char *signal)
{
    Q_D(QConnectionState);
    d->signal = signal;
}

const char *QConnectionState::signal() const
{
    Q_D(const QConnectionState);
    return d->signal;
}

void QConnectionState::setReceiver(QObject *receiver)
{
    Q_D(QConnectionState);
    d->receiver = receiver;
}

QObject *QConnectionState::receiver() const
{
    Q_D(const QConnectionState);
    return d->receiver;
}

void QConnectionState::setMember(const char *member)
{
    Q_D(QConnectionState);
    d->member = member;
}

const char *QConnectionState::member() const
{
    Q_D(const QConnectionState);
    return d->member;
}

void QConnectionState::setConnectionType(Qt::ConnectionType type)
{
    Q_D(QConnectionState);
    d->type = type;
}

Qt::ConnectionType QConnectionState::connectionType() const
{
    Q_D(const QConnectionState);
    return d->type;
}

QT_END_NAMESPACE

#include "moc_qconnectionstate_p.cpp"
