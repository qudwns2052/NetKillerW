#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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


private slots:
    void on_btn_select_clicked();
    void on_btn_deauth_clicked();

private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
