#include "mainwindow.h"
#include "gtrace.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qDebug() << "NetKillerW Start";

    w.show();

    return a.exec();
}
