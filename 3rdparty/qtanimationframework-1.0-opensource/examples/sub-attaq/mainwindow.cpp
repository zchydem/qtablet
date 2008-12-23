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
#include "mainwindow.h"
#include "graphicsscene.h"

#ifdef QT_OPENGL_SUPPORT
    #include <QtOpenGL/QtOpenGL>
#endif

MainWindow::MainWindow() : QMainWindow(0)
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *file = new QMenu(tr("&File"),menuBar);

    QAction *newAction = new QAction(tr("New Game"),file);
    newAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    file->addAction(newAction);
    QAction *pauseAction = new QAction(tr("Pause"),file);
    pauseAction->setShortcut(QKeySequence(Qt::Key_P));
    file->addAction(pauseAction);
    QAction *quitAction = new QAction(tr("Quit"),file);
    quitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    file->addAction(quitAction);

    menuBar->addMenu(file);
    setMenuBar(menuBar);

    connect(newAction,SIGNAL(triggered()),this,SLOT(onNewGameTriggered()));
    connect(pauseAction,SIGNAL(triggered()),this,SLOT(onPauseGameTriggered()));
    connect(quitAction,SIGNAL(triggered()),this,SLOT(onQuitGameTriggered()));

    QStringList list = QApplication::arguments();
    if (list.contains("-fullscreen")) {
        scene = new GraphicsScene(0, 0, 750, 400,GraphicsScene::Small);
        setWindowState(Qt::WindowFullScreen);
    } else {
        scene = new GraphicsScene(0, 0, 880, 630);
        layout()->setSizeConstraint(QLayout::SetFixedSize);
    }

    view = new QGraphicsView(scene,this);
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

#ifdef QT_OPENGL_SUPPORT
        view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    setCentralWidget(view);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onNewGameTriggered()
{
    if (!scene->gameInProgress()) {
        scene->initializeLevel();
    }
}

void MainWindow::onPauseGameTriggered()
{
     scene->pause();
}

void MainWindow::onQuitGameTriggered()
{
    qApp->closeAllWindows();
}
