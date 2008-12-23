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

#ifndef QSTATE_H
#define QSTATE_H

#include <QtCore/qobject.h>
#include <QtCore/qmetatype.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "common.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QGraphicsWidget;
class QPoint;
class QPointF;
class QRect;
class QRectF;
class QWidget;


class QtStateGroup;

class QtStatePrivate;
class Q_ANIMATION_EXPORT QtState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activated)
    Q_PROPERTY(bool restoreProperties READ restoreProperties WRITE setRestoreProperties)
    Q_PROPERTY(QtStateGroup* group READ group WRITE setGroup)

public:
    QtState(QObject *parent = 0);
    virtual ~QtState();

    virtual void clear();

    QtStateGroup *group() const;
    void setGroup(QtStateGroup *group);

    bool isActive() const;

    bool restoreProperties() const;
    void setRestoreProperties(bool enabled);

#ifndef QT_NO_PROPERTIES
    void setProperty(QObject *object, const char *propertyName, const QVariant &value);
    QVariant property(QObject *object, const char *propertyName) const;
    void unsetProperty(QObject *object, const char *propertyName);
    void setProperty(QGraphicsItem *item, const char *propertyName, const QVariant &value);
    QVariant property(QGraphicsItem *item, const char *propertyName) const;
    void unsetProperty(QGraphicsItem *item, const char *propertyName);
    void setProperty(QGraphicsWidget *item, const char *propertyName, const QVariant &value);
    QVariant property(QGraphicsWidget *item, const char *propertyName) const;
    void unsetProperty(QGraphicsWidget *item, const char *propertyName);
#endif

    // Convenience APIs
    // ### consider unsets for all?
    QPointF pos(QGraphicsItem *item) const;
    void setPos(QGraphicsItem *item, const QPointF &pos);
    QPoint pos(QWidget *widget) const;
    void setPos(QWidget *widget, const QPoint &pos);
    QRectF geometry(QGraphicsWidget *widget) const;
    void setGeometry(QGraphicsWidget *widget, const QRectF &geom);
    QRect geometry(QWidget *widget) const;
    void setGeometry(QWidget *widget, const QRect &geom);
    qreal opacity(QGraphicsItem *item) const;
    void setOpacity(QGraphicsItem *item, qreal opacity);

    // ### bad name
    // ### already connected, what to do?
    void setConnectionState(QObject *sender, const char *signal,
                            QObject *receiver, const char *member,
                            Qt::ConnectionType = Qt::AutoConnection);
    void unsetConnectionState(QObject *sender, const char *signal,
                              QObject *receiver, const char *member);

public Q_SLOTS:
    void setActive(bool active);
    void activate();
    void deactivate();

Q_SIGNALS:
    void activated(bool active);
    void activated();
    void deactivated();

protected:
    bool event(QEvent *event);
    virtual void enterState(QtState *fromState);
    virtual void leaveState(QtState *toState);

private:
    Q_DISABLE_COPY(QtState)
    Q_DECLARE_PRIVATE(QtState)
#ifdef QT_EXPERIMENTAL_SOLUTION
    QtStatePrivate *d_ptr;
#endif
    friend class QtStateGroup;
    friend class QtTransition;
    friend class QUiPropertyStateCreator;
    friend class QUiConnectionStateCreator;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QtState *)

QT_END_HEADER

#endif
