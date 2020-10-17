#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_all->setEnabled(0);
    ui->btn_stop->setEnabled(0);
    QHeaderView *verticalHeader = ui->tableWidget->verticalHeader();
    QHeaderView *verticalHeader2 = ui->tableWidget_2->verticalHeader();
    //    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    //    verticalHeader2->setSectionResizeMode(QHeaderView::Fixed);

#ifdef Q_OS_ANDROID
    verticalHeader->setDefaultSectionSize(100);
    verticalHeader2->setDefaultSectionSize(100);
#endif

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // start Server
    {
        system("export LD_PRELOAD=/system/lib/libfakeioctl.so");
        system("su -c \"nexutil -m2\"");
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

}

MainWindow::~MainWindow()
{
    system("su -c \"killall -9 deauthServer\"");
    delete ui;
}

void MainWindow::btn_ap_clicked()
{
    QPushButton *pb = qobject_cast<QPushButton *>(QObject::sender());
    int row = pb->property("my_key").toInt();

    if(ap_btn_list[row]->text() == "Stop")
    {
        scanThread_->active_ = false;
        scanThread_->quit();
        scanThread_->wait();
        delete scanThread_;

        for(auto it = station_btn_list.begin(); it != station_btn_list.end(); it++)
        {
            (*it)->setEnabled(1);
        }

        ap_btn_list[row]->setText("Find");

        for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
        {
            (*it)->setEnabled(1);
        }


        ui->btn_all->setEnabled(1);
        ui->btn_start->setEnabled(1);

        return;
    }

    for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
    {
        (*it)->setEnabled(0);
    }

    ap_btn_list[row]->setText("Stop");
    ap_btn_list[row]->setEnabled(1);

    station_map.clear();
    station_btn_list.clear();

    QStringList label = {"Mac address", "Signal", "Select"};

    ui->tableWidget_2->setRowCount(0);

    ui->tableWidget_2->setHorizontalHeaderLabels(label);

    scanThread_ = new ScanThread(client_sock, false);

    QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);


    memset(buf, 0x00, BUF_SIZE);
    memcpy(buf, "2", 1);
    send_data(client_sock, buf);

    memset(data, 0x00, BUF_SIZE);
    strcpy(data, ui->tableWidget->item(row, 1)->text().toStdString().c_str());
    send_data(client_sock, data);
    scanThread_->start();

    ui->btn_start->setEnabled(0);
}
void MainWindow::btn_station_clicked()
{
    QPushButton *pb = qobject_cast<QPushButton *>(QObject::sender());
    int row = pb->property("my_key").toInt();

    QMessageBox MsgBox;
    MsgBox.setWindowTitle("deauth Attack start !!!");
    MsgBox.setText(ui->tableWidget_2->item(row, 0)->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);


    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    memset(buf, 0x00, BUF_SIZE);

    memcpy(buf, "3", 1);
    if (write(client_sock, buf, 1) <= 0)
    {
        printf("write error\n");
        exit(1);
    }


    memset(data, 0x00, BUF_SIZE);
    strcpy(data, ui->tableWidget_2->item(row, 0)->text().toStdString().c_str());

    if (write(client_sock, data, strlen(data)) <= 0)
    {
        printf("write error\n");
        exit(1);
    }


}
void MainWindow::on_btn_all_clicked()
{
    QMessageBox MsgBox;
    MsgBox.setWindowTitle("All deauth Attack start !!!");
    MsgBox.setText("FF:FF:FF:FF:FF:FF");
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

    memset(buf, 0x00, BUF_SIZE);

    memcpy(buf, "4", 1);
    if (write(client_sock, buf, 1) <= 0)
    {
        printf("write error\n");
        exit(1);
    }

}


void MainWindow::on_btn_start_clicked()
{
    ap_map.clear();
    ap_btn_list.clear();

    QStringList label = {"SSID", "Mac address", "Select"};
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setHorizontalHeaderLabels(label);

    QStringList label2 = {"Mac address", "Signal", "Select"};
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setHorizontalHeaderLabels(label);

    scanThread_ = new ScanThread(client_sock, true);

    QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);


    memset(buf, 0x00, BUF_SIZE);
    memcpy(buf, "1", 1);
    send_data(client_sock, buf);

    scanThread_->start();

    ui->btn_start->setEnabled(0);
    ui->btn_stop->setEnabled(1);
}

void MainWindow::on_btn_stop_clicked()
{
    scanThread_->active_ = false;
    scanThread_->quit();
    scanThread_->wait();
    delete scanThread_;

    for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
    {
        (*it)->setEnabled(1);
    }

    ui->btn_start->setEnabled(1);
    ui->btn_stop->setEnabled(0);

}

void MainWindow::processCaptured(char* data)
{
    if(scanThread_->isAp)
    {
        QString temp = QString(data);

        QStringList info = temp.split("\t");


        if (ap_map.find(info[1]) != ap_map.end())
        {
            return;
        }

        ap_map[info[1]] = info[0];

        int row = ui->tableWidget->rowCount();

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(info[0]));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(info[1]));

        QPushButton * btn = new QPushButton(this);

        ap_btn_list.append(btn);

        btn->setProperty("my_key", row);
        btn->setText("Find");
        btn->setEnabled(0);

        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::btn_ap_clicked);
        ui->tableWidget->setCellWidget(row, 2, (QWidget*)btn);
    }


    else
    {
        QString temp = QString(data);
        QStringList info = temp.split("\t");

        int row = ui->tableWidget_2->rowCount();
        if (station_map.find(info[0]) != station_map.end())
        {
            return;
        }

        station_map[info[0]] = info[1];

        ui->tableWidget_2->insertRow(row);
        ui->tableWidget_2->setItem(row, 0, new QTableWidgetItem(info[0]));
        ui->tableWidget_2->setItem(row, 1, new QTableWidgetItem(info[1]));

        QPushButton * btn = new QPushButton(this);

        station_btn_list.append(btn);

        btn->setProperty("my_key", row);
        btn->setText("Attack");
        btn->setEnabled(0);

        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::btn_station_clicked);
        ui->tableWidget_2->setCellWidget(row,2,(QWidget*)btn);

    }
}
//        char * ptr;
//        int i=0;

//        if(strlen(data) != 0)
//        {
//            MsgBox.setWindowTitle("Success Get Client info !");
//            ptr = strtok(data, ",");
//            while (ptr != NULL)
//            {
//                QString temp = QString(ptr);
//                QStringList list = temp.split("\t");

//                ui->tableWidget_2->insertRow(ui->tableWidget_2->rowCount());
//                ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(list[0]));
//                ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(list[1]));

//                QPushButton * btn = new QPushButton(this);

//                btn->setProperty("my_key", i);
//                btn->setText("Attack");

//                QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::btn_station_clicked);
//                ui->tableWidget_2->setCellWidget(i,2,(QWidget*)btn);

//                ptr = strtok(NULL, ",");
//                i++;
//            }
//        }
//        else
//        {
//            MsgBox.setWindowTitle("Failed Get Client info ...");
//        }

//        MsgBox.setStandardButtons(QMessageBox::Ok);
//        MsgBox.setDefaultButton(QMessageBox::Ok);

//        if ( MsgBox.exec() != QMessageBox::Ok )
//        {
//            printf("Msg error\n");
//            exit(1);
//        }

//    }


