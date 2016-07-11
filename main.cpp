//#include "mainwindow.h"
#include <QApplication>
#include "page.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    OutPage page;
    page.show();

    return a.exec();
}
