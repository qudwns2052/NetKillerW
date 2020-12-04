#include "mainwindow.h"
#include "gtrace.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    gtrace_close();
    gtrace_open("127.0.0.1", 8908, false, "netkillerw_ca.log");

    GTRACE("NetKillerW Start");

    w.show();

    return a.exec();
}
