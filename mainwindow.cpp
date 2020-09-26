#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_select->setEnabled(1);
    ui->btn_deauth->setEnabled(1);
    ui->textBrowser->setText("");

    // start Server
    {
        system("su -c \"/data/local/tmp/gilgil/deauthServer&\"");
        sleep(1);
    }


    int server_port = 9998;

    //socket connection
    {
        if(!connect_sock(&client_sock, server_port))
        {
            printf("connection error\n");
            exit(1);
        }

        printf("connection ok\n");
    }

    {
        memset(data, 0x00, BUF_SIZE);

        if (read(client_sock, data, BUF_SIZE) <= 0)
        {
            printf("read error\n");
            exit(1);
        }


        char *ptr = strtok(data, ",");
        while (ptr != NULL)
        {
            ui->listWidget->addItem(ptr);
            ptr = strtok(NULL, ",");
        }

    }


}

MainWindow::~MainWindow()
{
    system("su -c \"killall -9 deauthServer\"");
    delete ui;
}


void MainWindow::on_btn_select_clicked()
{

    QMessageBox MsgBox;
    MsgBox.setWindowTitle("Selected AP");
    MsgBox.setText(ui->listWidget->currentItem()->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    ui->textBrowser->append("Selected AP ...");
    ui->btn_deauth->setEnabled(1);

    memset(data, 0x00, BUF_SIZE);

    strcpy(data, ui->listWidget->currentItem()->text().toStdString().c_str());


//    ui->listWidget->clear();

//    memset(data, 0x00, BUF_SIZE);

//    if (read(client_sock, data, BUF_SIZE) <= 0)
//    {
//        printf("read error\n");
//        exit(1);
//    }

//    char *ptr = strtok(data, ",");
//    while (ptr != NULL)
//    {
//        ui->listWidget->addItem(ptr);
//        ptr = strtok(NULL, ",");
//    }
}


void MainWindow::on_btn_deauth_clicked()
{
    QMessageBox MsgBox;
    MsgBox.setWindowTitle("deauth Attack start !!!");
    MsgBox.setText(ui->listWidget->currentItem()->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    ui->textBrowser->append("deauth attack ...");

    char *ptr = strtok(data, "@");
    ptr = strtok(NULL, "@");


    if (write(client_sock, ptr, strlen(ptr)) <= 0)
    {
        printf("write error\n");
        exit(1);
    }

    ui->btn_deauth->setEnabled(0);
}
