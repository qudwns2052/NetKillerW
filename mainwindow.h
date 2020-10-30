#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include <QScrollBar>
#include <QFontDatabase>
#include "socket.h"
#include "thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


typedef struct ap_map_info
{
    int channel;
    QMap <QString, QString> station_map;
}ap_info;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int client_sock;
    char data[BUF_SIZE];
    char buf[BUF_SIZE];
    QString selected_ap;

    QList<QPushButton *> ap_btn_list;
    QList<QPushButton *> station_btn_list;

    QMap <QString, ap_map_info> ap_map;

    ScanThread * scanThread_;

private slots:
    void btn_ap_clicked();
    void btn_station_clicked();
    void processCaptured(char* data);
    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
