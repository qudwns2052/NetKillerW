#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include "socket.h"
#include "thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int client_sock;
    char data[BUF_SIZE];
    char buf[BUF_SIZE];
    QList<QPushButton *> ap_btn_list;
    QList<QPushButton *> station_btn_list;

    QMap <QString, QString> ap_map;
    QMap <QString, QString> station_map;

    ScanThread * scanThread_;

private slots:
    void btn_ap_clicked();
    void btn_station_clicked();
    void on_btn_all_clicked();
    void on_btn_start_clicked();
    void processCaptured(char* data);

    void on_btn_stop_clicked();

private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
