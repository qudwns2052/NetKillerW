#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <socket.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static const int BUF_SIZE=1024;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int client_sock;
    char data[BUF_SIZE];
    char buf[BUF_SIZE];
    char selected_ap[BUF_SIZE];
    std::vector<std::string> selected_client;
    QList<QPushButton *> btn_list;

private slots:
    void on_btn_all_clicked();
    void on_tableWidget_cellClicked(int row, int column);
    void on_tableWidget_2_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
