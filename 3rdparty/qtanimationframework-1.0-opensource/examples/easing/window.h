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

#include <QtGui>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include <common.h>
# include <qanimation.h>
#endif

class Window : public QWidget {
    Q_OBJECT
public:
    Window(QWidget *parent = 0) 
        : QWidget(parent), m_iconSize(64, 64)
    {
        setWindowTitle(tr("Easing curves"));
        m_listWidget = new QListWidget;
        m_listWidget->setViewMode(QListView::IconMode);
        m_listWidget->setIconSize(m_iconSize);  
        m_listWidget->setMovement(QListView::Static);
        m_listWidget->setFlow(QListView::TopToBottom);
        m_listWidget->setWrapping(false);
        m_listWidget->setMaximumWidth(m_iconSize.width() + 32);
        
        connect(m_listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(curveChanged(int)));
        createCurveIcons();

        QPixmap pix(QLatin1String(":/images/qt-logo.png"));
        m_item = new QGraphicsPixmapItem(pix);
        m_scene.addItem(m_item);
        m_graphicsView = new QGraphicsView(&m_scene);

        QHBoxLayout *lay = new QHBoxLayout;
        lay->addWidget(m_listWidget);
        lay->addWidget(m_graphicsView);
        setLayout(lay);

        m_anim = new QtAnimation(m_item, "pos");
        startAnimation();
    }
private slots:
    void curveChanged(int row);
    
private:
    void createCurveIcons();
    void startAnimation();
    
    QListWidget *m_listWidget;
    QPushButton *m_button;
    QGraphicsView *m_graphicsView;
    QGraphicsScene m_scene;
    QGraphicsPixmapItem *m_item;
    QtAnimation *m_anim;
    QSize m_iconSize;


};
