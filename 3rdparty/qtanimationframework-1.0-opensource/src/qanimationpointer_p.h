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

#ifndef QANIMATIONPOINTER_P_H
#define QANIMATIONPOINTER_P_H

#include <QtCore/qhash.h>
#include <QtCore/QObject>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsWidget>
QT_BEGIN_NAMESPACE

class QGraphicsItem;
class QGraphicsWidget;
class QObject;

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "common.h"
#endif

// ### Qt 5: Remove this class, provide common base for QGraphicsItem and QObject.
class QtAnimationPointer
{
public:
    QtAnimationPointer()
        : o(0), isObject(false) { }
    QtAnimationPointer(QObject *o)
        : o(o), isObject(true) { }
    QtAnimationPointer(QGraphicsItem *i)
        : i(i), isObject(false) { }
    QtAnimationPointer(QGraphicsWidget *w)
        : o((QObject *)w), isObject(true) { }
    QtAnimationPointer(const QtAnimationPointer &other)
        { i = other.i; isObject = other.isObject; }
    bool operator==(const QtAnimationPointer &other) const
        { return i == other.i; }
    bool operator==(QObject *o) const
        { return this->o == o; }
    bool operator==(QGraphicsItem *i) const
        { return this->i == i; }
    bool operator==(QGraphicsWidget *w) const
        { return this->o == (QObject *)w; }
    bool operator!=(const QtAnimationPointer &other) const
        { return !operator==(other); }
    bool operator!=(QObject *o) const
        { return !operator==(o); }
    bool operator!=(QGraphicsItem *i) const
        { return !operator==(i); }
    bool operator!=(QGraphicsWidget *w) const
        { return !operator==(w); }
    bool operator<(const QtAnimationPointer &other) const
        { return i < other.i; }
    bool operator!() const
        { return i == 0; }

    QtAnimationPointer &operator=(QObject *o)
        { this->o = o; isObject = true; return *this; }
    QtAnimationPointer &operator=(QGraphicsItem *i)
        { this->i = i; isObject = false; return *this; }
    QtAnimationPointer &operator=(QGraphicsWidget *w)
        { this->o = (QObject *)w; isObject = true; return *this; }

    operator QObject*() const { return object(); }
    operator QGraphicsItem*() const { return item(); }

    QObject *object() const { return isObject ? o : 0; }
    QGraphicsItem *item() const { return isObject ? 0 : i; }

private:
    union {
        QGraphicsItem *i;
        QObject *o;
    };
    bool isObject;
};

inline uint qHash(const QtAnimationPointer &ptr)
{
    if (ptr.object())
        return qHash(ptr.object());
    return qHash(ptr.item());
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QtAnimationPointer)

#endif
