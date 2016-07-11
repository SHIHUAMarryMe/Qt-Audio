#include "mainwindow.h"
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <memory>
#include <QMenuBar>
#include <QDebug>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
           :QMainWindow(parent)
{
    this->setMinimumSize(QSize(1080, 500));
    this->addActionsToWindow();
    this->addMenusToWindow();
    this->addTabWidget();
}



void MainWindow::showAbout()noexcept
{
    QMessageBox::about(this, tr("About"), tr("About Me"));
}

void MainWindow::addActionsToWindow()noexcept
{
    this->aboutAction = std::shared_ptr<QAction>(new QAction(tr("&About"), this));
    connect((this->aboutAction).get(), &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::addMenusToWindow()noexcept
{
    qDebug()<<"success";
    this->menuBar = std::shared_ptr<QMenuBar>(new QMenuBar(this));
    this->aboutMenu = std::shared_ptr<QMenu>(new QMenu(tr("About"),this));
    (this->aboutMenu)->addAction(aboutAction.get());
    (this->menuBar)->addMenu(aboutMenu.get());
    this->setMenuBar(menuBar.get());
}


void MainWindow::addTabWidget()noexcept
{
    /*this->out1 = std::shared_ptr<QPushButton>(new QPushButton(tr("out 1")));
    this->widget1 = std::shared_ptr<QWidget>(new QWidget);
    this->tabWidget = std::shared_ptr<QTabWidget>(new QTabWidget(this));
    (this->tabWidget)->addTab(widget1.get(), tr("out 1"));*/

    /*QDockWidget* dockWidget = new QDockWidget(tr("DockWidget"));
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);*/

    QWidget* widget1 = new QWidget;
    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->addTab(widget1, tr("out1"));
    this->setCentralWidget(tabWidget);

}

