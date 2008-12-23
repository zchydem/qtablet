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

#ifndef QPAUSEANIMATION_P_H
#define QPAUSEANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtAnimationGroup. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qabstractanimation.h"

QT_BEGIN_NAMESPACE

class QPauseAnimation : public QtAbstractAnimation
{
    Q_OBJECT
public:
    QPauseAnimation(int msecs, QObject *parent = 0)
        : QtAbstractAnimation(parent), pause(msecs)
    { }

    int duration() const
    { return pause; }

protected:
    void updateCurrentTime(int msecs)
    { Q_UNUSED(msecs); }

private:
    int pause;
};

QT_END_NAMESPACE

#endif
