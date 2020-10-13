#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_select->setEnabled(1);
    ui->btn_all->setEnabled(0);
    ui->btn_deauth->setEnabled(0);

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
    MsgBox.setWindowTitle("Get Client info. Please Wait 5 seconds.");
    MsgBox.setText(ui->listWidget->currentItem()->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    ui->btn_all->setEnabled(1);
    ui->btn_deauth->setEnabled(1);

    memset(selected_ap, 0x00, BUF_SIZE);
    strcpy(selected_ap, ui->listWidget->currentItem()->text().toStdString().c_str());

    char *ptr = strtok(selected_ap, "\t");
    ptr = strtok(NULL, "\t");

    if (write(client_sock, ptr, strlen(ptr)) <= 0)
    {
        printf("write error\n");
        exit(1);
    }

    memset(data, 0x00, BUF_SIZE);

    if (read(client_sock, data, BUF_SIZE) <= 0)
    {
        printf("read error\n");
        exit(1);
    }

    if(strlen(data) != 0)
    {
        MsgBox.setWindowTitle("Success Get Client info !");
        ptr = strtok(data, ",");
        while (ptr != NULL)
        {
            ui->listWidget_2->addItem(ptr);
            ptr = strtok(NULL, ",");
        }

    }
    else
    {
        MsgBox.setWindowTitle("Failed Get Client info ...");

    }
    MsgBox.setText(ui->listWidget->currentItem()->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

}

void MainWindow::on_btn_all_clicked()
{
    QMessageBox MsgBox;
    MsgBox.setWindowTitle("select All Client");
    MsgBox.setText(ui->listWidget->currentItem()->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    memset(data, 0x00, BUF_SIZE);
    strcpy(data, ui->listWidget_2->currentItem()->text().toStdString().c_str());


//    ui->listWidget_2->item(i)
//    char *ptr = strtok(data, "@");
//    ptr = strtok(NULL, "@");


//    if (write(client_sock, ptr, strlen(ptr)) <= 0)
//    {
//        printf("write error\n");
//        exit(1);
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


    memset(data, 0x00, BUF_SIZE);
    strcpy(data, ui->listWidget_2->currentItem()->text().toStdString().c_str());

    char *ptr = strtok(data, "\t");


    if (write(client_sock, ptr, strlen(ptr)) <= 0)
    {
        printf("write error\n");
        exit(1);
    }

}

