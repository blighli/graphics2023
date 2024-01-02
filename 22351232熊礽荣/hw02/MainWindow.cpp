#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    fullscreen = true;
    ui.setupUi(this);
    setFixedSize(1200, 800);
    setWindowTitle(tr("homework02"));
    mainWidget = new MainWidget(this);
    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{}


void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_F2:
        fullscreen = !fullscreen;
        if (fullscreen)
	        showFullScreen();
        else
	        showNormal();
        mainWidget->update();
        break;
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_W:
        mainWidget->eyeXup();
        break;
    case Qt::Key_S:
        mainWidget->eyeXdown();
        break;
    case Qt::Key_E:
        mainWidget->eyeZup();
        break;
    case Qt::Key_D:
        mainWidget->eyeZdown();
        break;
    default:
    	qDebug() << mainWidget->eyeX << " " << mainWidget->eyeY << " " << mainWidget->eyeZ;
    }
}