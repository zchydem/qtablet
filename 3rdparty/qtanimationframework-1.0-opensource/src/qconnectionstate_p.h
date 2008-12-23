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

#ifndef QCONNECTIONSTATE_P_H
#define QCONNECTIONSTATE_P_H

#include <QtCore/qobject.h>
QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtState. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class QConnectionStatePrivate;

class QConnectionState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* sender READ sender WRITE setSender)
    Q_PROPERTY(const char* signal READ signal WRITE setSignal)
    Q_PROPERTY(QObject* receiver READ receiver WRITE setReceiver)
    Q_PROPERTY(const char* member READ member WRITE setMember)
    Q_PROPERTY(Qt::ConnectionType type READ connectionType WRITE setConnectionType)

public:
    QConnectionState(QObject *parent = 0);
    explicit QConnectionState(QObject *sender, const char *signal,
                              QObject *receiver, const char *member,
                              Qt::ConnectionType type = Qt::AutoConnection,
                              QObject *parent = 0);

    void setSender(QObject *sender);
    QObject *sender() const;

    void setSignal(const char *signal);
    const char *signal() const;

    void setReceiver(QObject *receiver);
    QObject *receiver() const;

    void setMember(const char *member);
    const char *member() const;

    void setConnectionType(Qt::ConnectionType type);
    Qt::ConnectionType connectionType() const;

#ifdef QT_EXPERIMENTAL_SOLUTION
    QConnectionStatePrivate *d_ptr;
#endif

private:
    Q_DECLARE_PRIVATE(QConnectionState);
    Q_DISABLE_COPY(QConnectionState);
};

QT_END_NAMESPACE

#endif
