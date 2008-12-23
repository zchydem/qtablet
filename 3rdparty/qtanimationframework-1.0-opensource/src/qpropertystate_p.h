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

#ifndef QPROPERTYSTATE_P_H
#define QPROPERTYSTATE_P_H

#include <QtCore/qobject.h>

#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qanimationpointer_p.h>
#else
#include <qanimationpointer_p.h>
#endif
QT_BEGIN_NAMESPACE

class QGraphicsItem;
class QGraphicsWidget;

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

class QPropertyStatePrivate;

class QPropertyState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtAnimationPointer target READ target WRITE setTarget)
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QGraphicsItem* targetItem READ targetItem WRITE setTargetItem)
    Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    QPropertyState(QObject *parent = 0);
    explicit QPropertyState(QtAnimationPointer target, const QByteArray &propertyName, const QVariant &value, QObject *parent = 0);
    ~QPropertyState();

    void setTarget(const QtAnimationPointer &target);
    QtAnimationPointer target() const;

    void setTargetObject(QObject *target);
    QObject *targetObject() const;
    
    void setTargetItem(QGraphicsItem *target);
    QGraphicsItem *targetItem() const;
    
    void setPropertyName(const QByteArray &propertyName);
    QByteArray propertyName() const;

    void setValue(const QVariant &value);
    QVariant value() const;

#ifdef QT_EXPERIMENTAL_SOLUTION
    QPropertyStatePrivate *d_ptr;
#endif

Q_SIGNALS:
    void valueChanged(const QVariant &value, const QVariant &oldValue);

private:
    void setPropertyName(const QString &propertyName);
    
    Q_DECLARE_PRIVATE(QPropertyState);
    Q_DISABLE_COPY(QPropertyState);
};

QT_END_NAMESPACE

#endif
