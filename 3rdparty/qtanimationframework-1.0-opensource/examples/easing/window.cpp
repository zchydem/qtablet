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

#include "window.h"

void Window::createCurveIcons()
{
    static const char* names[]= {
        "Linear",
        "InQuad", "OutQuad", "InOutQuad", "OutInQuad",
        "InCubic", "OutCubic", "InOutCubic", "OutInCubic",
        "InQuart", "OutQuart", "InOutQuart", "OutInQuart",
        "InQuint", "OutQuint", "InOutQuint", "OutInQuint",
        "InSine", "OutSine", "InOutSine", "OutInSine",
        "InExpo", "OutExpo", "InOutExpo", "OutInExpo",
        "InCirc", "OutCirc", "InOutCirc", "OutInCirc",
        "InElastic", "OutElastic", "InOutElastic", "OutInElastic",
        "InBack", "OutBack", "InOutBack", "OutInBack",
        "InBounce", "OutBounce", "InOutBounce", "OutInBounce",
        "InCurve", "OutCurve", "SineCurve", "CosineCurve"
        };

    QPixmap pix(m_iconSize);
    QPainter painter(&pix);
    QLinearGradient gradient(0,0, 0, m_iconSize.height());
    gradient.setColorAt(0.0, QColor(240, 240, 240));
    gradient.setColorAt(1.0, QColor(224, 224, 224));
    QBrush brush(gradient);
    for (int i = QtEasingCurve::Linear; i <= QtEasingCurve::CosineCurve; ++i) {
        painter.fillRect(QRect(QPoint(0, 0), m_iconSize), brush);
        QtEasingCurve curve((QtEasingCurve::Type)i);
        painter.setPen(QColor(0, 0, 255, 64));
        qreal xAxis = m_iconSize.height()/1.5;
        qreal yAxis = m_iconSize.width()/3;
        painter.drawLine(0, xAxis, m_iconSize.width(),  xAxis);
        painter.drawLine(yAxis, 0, yAxis, m_iconSize.height());
        painter.setPen(Qt::black);
        
        qreal curveScale = m_iconSize.height()/2;
        QPoint currentPos(yAxis, xAxis);
        
        for (qreal t = 0; t < 1.0; t+=1.0/curveScale) {
            QPoint to;
            to.setX(yAxis + curveScale * t);
            to.setY(xAxis - curveScale * curve.valueForStep(t));
            painter.drawLine(currentPos, to);
            currentPos = to;
        }
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(QIcon(pix));
        item->setText(QLatin1String(names[i]));
        m_listWidget->addItem(item);
    }
}


void Window::startAnimation()
{
    m_anim->setStartValue(QPointF(0, 0));
    m_anim->setTargetValue(QPointF(0, 200));
    m_anim->setDuration(2000);
    m_anim->setLoopCount(-1.0); // forever
    m_anim->start();
}

void Window::curveChanged(int row)
{
    m_anim->setEasingCurve((QtEasingCurve::Type)row);
    m_anim->setCurrentTime(0);  // ### restart bug?
}
