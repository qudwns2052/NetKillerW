#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_all->setEnabled(0);

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

    {
        memset(buf, 0x00, BUF_SIZE);

        memcpy(buf, "1", 1);
        if (write(client_sock, buf, 1) <= 0)
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


        char *ptr = strtok(data, ",");
        int i=0;
        while (ptr != NULL)
        {
            QString temp = QString(ptr);
            QStringList list = temp.split("\t");


            ui->tableWidget->insertRow(ui->tableWidget->rowCount() );
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(list[0]));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(list[1]));

            //            QPushButton * btn = new QPushButton();
            //            btn_list.append(new QPushButton());
            //            btn_list[i]->setText("select");

            //            btn->setText("Select");
            //            btn_list.append(btn);

            //            QObject::connect(btn_list[i], &QPushButton::clicked, this, &MainWindow::ap_clicked);
            //            ui->tableWidget->setCellWidget(i,2,(QWidget*)btn_list[i]);


            ptr = strtok(NULL, ",");
            i++;
        }

        //ui->tableWidget->resizeColumnsToContents();
        //        ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    }


}

MainWindow::~MainWindow()
{
    system("su -c \"killall -9 deauthServer\"");
    delete ui;
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


void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    //    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    //    int row = ui->tableWidget->indexAt(w->pos()).row();
    //    QPushButton *pb = qobject_cast<QPushButton *>(QObject::sender());
    //    int row = pb->objectName().toInt();
    //    QString row_str = QString(row);

    QMessageBox MsgBox;
    MsgBox.setWindowTitle("Get Client info. Please Wait 5 seconds.");
    MsgBox.setText(ui->tableWidget->item(row, 0)->text());
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }


    ui->btn_all->setEnabled(1);


    memset(buf, 0x00, BUF_SIZE);

    memcpy(buf, "2", 1);
    if (write(client_sock, buf, 1) <= 0)
    {
        printf("write error\n");
        exit(1);
    }


    memset(data, 0x00, BUF_SIZE);
    strcpy(data, ui->tableWidget->item(row, 1)->text().toStdString().c_str());

    if (write(client_sock, data, strlen(data)) <= 0)
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

    //    QStringList label = {"Mac", "Signal"};

    ui->tableWidget_2->setRowCount(0);
    //    QHeaderView *verticalHeader2 = ui->tableWidget_2->verticalHeader();
    //    verticalHeader2->setDefaultSectionSize(100);
    //    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    //    ui->tableWidget_2->setHorizontalHeaderLabels(label);

    char * ptr;
    int i=0;

    if(strlen(data) != 0)
    {
        MsgBox.setWindowTitle("Success Get Client info !");
        ptr = strtok(data, ",");
        while (ptr != NULL)
        {
            QString temp = QString(ptr);
            QStringList list = temp.split("\t");

            ui->tableWidget_2->insertRow(ui->tableWidget_2->rowCount());
            ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(list[0]));
            ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(list[1]));

            ptr = strtok(NULL, ",");
            i++;
        }
    }
    else
    {
        MsgBox.setWindowTitle("Failed Get Client info ...");
    }

//    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(100);
//    ui->tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget_2->resizeColumnsToContents();

    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.setDefaultButton(QMessageBox::Ok);

    if ( MsgBox.exec() != QMessageBox::Ok )
    {
        printf("Msg error\n");
        exit(1);
    }

}

void MainWindow::on_tableWidget_2_cellClicked(int row, int column)
{
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
