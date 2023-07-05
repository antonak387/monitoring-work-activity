#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)// конструктор
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      server(nullptr)
{
    ui->setupUi(this);

    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &MainWindow::newConnection);

    if (!server->listen(QHostAddress::Any, 1234))
    {
        qDebug() << "Server could not start!";
        return;
    }

    qDebug() << "Server started!";

    ui->label->setText("Number of connected clients: 0");
}

MainWindow::~MainWindow()// деструктор
{
    delete ui;
}

void MainWindow::newConnection()//новое соединение
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *clientSocket = server->nextPendingConnection();
        clients.append(clientSocket);

        connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::processData);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);

        qDebug() << "New client connected!";

        ui->label->setText(QString("Number of connected clients: %1").arg(clients.count()));

        ui->tableWidget->setRowCount(clients.count());

    }
}

void MainWindow::processData()//получили сообщение от клиента
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        QByteArray data = clientSocket->readAll();
        qDebug() << "Received data from client:" << data;

        // Разделение данных
        QList<QByteArray> dataList = data.split(',');

        if (dataList.size() >= 5)
        {
            QString domain = QString::fromUtf8(dataList[0]);
            QString computerName = QString::fromUtf8(dataList[1]);
            QString ipAddress = QString::fromUtf8(dataList[2]);
            QString userName = QString::fromUtf8(dataList[3]);
            QString clientTime = QString::fromUtf8(dataList[4]);

            qDebug() << "Domain:" << domain;
            qDebug() << "Computer Name:" << computerName;
            qDebug() << "IP Address:" << ipAddress;
            qDebug() << "User Name:" << userName;
            qDebug() << "clientTime:" << clientTime;


            if (ui->tableWidget->item(numClient,0)==nullptr)
            {
                QTableWidgetItem * i; //Создали указатель
                i = new QTableWidgetItem;
                ui->tableWidget->setItem(numClient,0,i);
            }
            ui->tableWidget->item(numClient,0)->setText(domain);

            if (ui->tableWidget->item(numClient,1)==nullptr)
            {
                QTableWidgetItem * i; //Создали указатель
                i = new QTableWidgetItem;
                ui->tableWidget->setItem(numClient,1,i);
            }
            ui->tableWidget->item(numClient,1)->setText(computerName);

            if (ui->tableWidget->item(numClient,2)==nullptr)
            {
                QTableWidgetItem * i; //Создали указатель
                i = new QTableWidgetItem;
                ui->tableWidget->setItem(numClient,2,i);
            }
            ui->tableWidget->item(numClient,2)->setText(ipAddress);

            if (ui->tableWidget->item(numClient,3)==nullptr)
            {
                QTableWidgetItem * i; //Создали указатель
                i = new QTableWidgetItem;
                ui->tableWidget->setItem(numClient,3,i);
            }
            ui->tableWidget->item(numClient,3)->setText(userName);

            if (ui->tableWidget->item(numClient,4)==nullptr)
            {
                QTableWidgetItem * i; //Создали указатель
                i = new QTableWidgetItem;
                ui->tableWidget->setItem(numClient,4,i);
            }
            ui->tableWidget->item(numClient,4)->setText(clientTime);

            numClient++;
            //clientId = new QString[numClient];
            //clientId[numClient] = domain+computerName+ipAddress+userName;
            //qDebug() << "clientId[numClient]:" << clientId[numClient];

        }

        clientSocket->disconnectFromHost();
    }
}
