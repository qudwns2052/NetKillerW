#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHeaderView *verticalHeader = ui->tableWidget->verticalHeader();
    QHeaderView *verticalHeader2 = ui->tableWidget_2->verticalHeader();
    //    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    //    verticalHeader2->setSectionResizeMode(QHeaderView::Fixed);

#ifdef Q_OS_ANDROID
    verticalHeader->setDefaultSectionSize(60);
    verticalHeader2->setDefaultSectionSize(60);
#endif

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 70px; }");
    ui->tableWidget_2->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 70px; }");

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


    QStringList label = {"SSID", "Mac address", "Select"};
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setHorizontalHeaderLabels(label);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    int column_width = ui->tableWidget->columnWidth(0) / 5;

//    ui->tableWidget->setColumnWidth(0, column_width * 2);
//    ui->tableWidget->setColumnWidth(1, column_width * 2);
//    ui->tableWidget->setColumnWidth(1, column_width);
    //ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    scanThread_ = new ScanThread(client_sock);

    QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);

    scanThread_->start();
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

    if(ap_btn_list[row]->text() != "Stop")
    {
        ap_btn_list[row]->setText("Stop");

        scanThread_->active_ = false;
        scanThread_->quit();
        scanThread_->wait();
        delete scanThread_;

        for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
        {
            (*it)->setEnabled(0);
        }

        for(auto it = station_btn_list.begin(); it != station_btn_list.end(); it++)
        {
            (*it)->setEnabled(0);
        }

        for (int i=0; i< ui->tableWidget_2->rowCount(); i++)
        {
            ui->tableWidget_2->item(i, 0)->setBackgroundColor(Qt::red);
            ui->tableWidget_2->item(i, 1)->setBackgroundColor(Qt::red);
        }

        ap_btn_list[row]->setEnabled(1);

    }
    else
    {
        ap_btn_list[row]->setText("Attack");
        scanThread_ = new ScanThread(client_sock);

        QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);

        scanThread_->start();

        for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
        {
            (*it)->setEnabled(1);
        }

        for(auto it = station_btn_list.begin(); it != station_btn_list.end(); it++)
        {
            (*it)->setEnabled(1);
        }

        for (int i=0; i< ui->tableWidget_2->rowCount(); i++)
        {
            ui->tableWidget_2->item(i, 0)->setBackgroundColor(Qt::white);
            ui->tableWidget_2->item(i, 1)->setBackgroundColor(Qt::white);
        }

        usleep(500000);
    }

    memset(buf, 0x00, BUF_SIZE);
    memcpy(buf, "3", 1);
    send_data(client_sock, buf);

    memset(data, 0x00, BUF_SIZE);
    QString temp = ui->tableWidget->item(row, 1)->text() + "\t" + "FF:FF:FF:FF:FF:FF";
    strcpy(data, temp.toStdString().c_str());
    send_data(client_sock, data);


}
void MainWindow::btn_station_clicked()
{
    QPushButton *pb = qobject_cast<QPushButton *>(QObject::sender());
    int row = pb->property("my_key").toInt();

    QString selected_ap;

    for (auto it = ap_map.begin(); it != ap_map.end(); it++)
    {
        for (auto i = it->station_map.begin(); i != it->station_map.end(); i++)
        {
            if (i.key() == ui->tableWidget_2->item(row, 0)->text())
            {
                selected_ap = it.key();
                break;
            }
        }
    }

    if(station_btn_list[row]->text() != "Stop")
    {
        station_btn_list[row]->setText("Stop");

        if (scanThread_->active_)
        {
            scanThread_->active_ = false;
            scanThread_->quit();
            scanThread_->wait();
            delete scanThread_;

            for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
            {
                //                if(ui->tableWidget->item((*it)->property("my_key").toInt(), 1)->text() != selected_ap)
                (*it)->setEnabled(0);
            }
        }

        ui->tableWidget_2->item(row, 0)->setBackgroundColor(Qt::red);
        ui->tableWidget_2->item(row, 1)->setBackgroundColor(Qt::red);
    }
    else
    {
        station_btn_list[row]->setText("Attack");
        station_btn_list[row]->setEnabled(1);

        bool isAttack = false;
        for(auto it = station_btn_list.begin(); it != station_btn_list.end(); it++)
        {
            if ((*it)->text() == "Stop")
            {
                isAttack = true;
                break;
            }
        }
        if(!isAttack)
        {
            scanThread_ = new ScanThread(client_sock);

            QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);

            scanThread_->start();

            for(auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
            {
                (*it)->setEnabled(1);
            }

            usleep(500000);
        }

        ui->tableWidget_2->item(row, 0)->setBackgroundColor(Qt::white);
        ui->tableWidget_2->item(row, 1)->setBackgroundColor(Qt::white);
    }

    memset(buf, 0x00, BUF_SIZE);
    memcpy(buf, "3", 1);
    send_data(client_sock, buf);

    memset(data, 0x00, BUF_SIZE);
    QString temp = selected_ap + "\t" + ui->tableWidget_2->item(row, 0)->text();
    strcpy(data, temp.toStdString().c_str());
    send_data(client_sock, data);

}

void MainWindow::processCaptured(char* data)
{

    QString temp = QString(data);
    QStringList info = temp.split("\t");

    if(info[0] == "1") // if ap info
    {
        int row = ui->tableWidget->rowCount();
        if (ap_map.find(info[2]) != ap_map.end())
        {
            return;
        }

        // 1\tgoka_5g\t12:34:56\tchannel

        ap_map[info[2]].channel = info[3].toInt();


        ui->tableWidget->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem(info[1]);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, item);
        QTableWidgetItem *item2 = new QTableWidgetItem(info[2]);
        item2->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, item2);

        QPushButton * btn = new QPushButton(this);

        ap_btn_list.append(btn);

        btn->setProperty("my_key", row);
        btn->setText("Attack");
        btn->setEnabled(1);

        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::btn_ap_clicked);
        ui->tableWidget->setCellWidget(row, 2, (QWidget*)btn);

    }
    else if (info[0] == "2")
    {
        if(ap_map[info[1]].station_map.find(info[2]) != ap_map[info[1]].station_map.end())
        {
            return;
        }

        // 2\tAPmac\tStationMAC\t-58
        ap_map[info[1]].station_map[info[2]] = info[3];

        int ap_row;
        for (int i=0; i<ui->tableWidget->rowCount(); i++)
        {
            if(ui->tableWidget->item(i, 1)->text() == info[1])
            {
                ap_row = i;
                break;
            }
        }

        ui->tableWidget->item(ap_row, 0)->setBackgroundColor(Qt::green);
        ui->tableWidget->item(ap_row, 1)->setBackgroundColor(Qt::green);

    }
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{

    station_btn_list.clear();

    selected_ap = ui->tableWidget->item(row, 1)->text();

    for (auto it = ap_btn_list.begin(); it != ap_btn_list.end(); it++)
    {
        (*it)->setStyleSheet("background-color: white");
    }
    ap_btn_list[row]->setStyleSheet("background-color: cyan");



    QStringList label = {"Mac address", "Signal", "Select"};
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setHorizontalHeaderLabels(label);


    QMapIterator<QString, QString> i(ap_map[selected_ap].station_map);

    int row2;

    while (i.hasNext()) {
        i.next();
        row2 = ui->tableWidget_2->rowCount();

        ui->tableWidget_2->insertRow(row2);
        QTableWidgetItem *item = new QTableWidgetItem(i.key());
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_2->setItem(row2, 0, item);
        QTableWidgetItem *item2 = new QTableWidgetItem(i.value());
        item2->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_2->setItem(row2, 1, item2);

        QPushButton * btn = new QPushButton(this);

        station_btn_list.append(btn);


        btn->setProperty("my_key", row2);
        btn->setText("Attack");
        btn->setEnabled(1);

        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::btn_station_clicked);
        ui->tableWidget_2->setCellWidget(row2, 2, (QWidget*)btn);
    }


}
