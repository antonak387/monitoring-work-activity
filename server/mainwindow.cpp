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

        if (dataList.size() >= 4)
        {
            QString domain = QString::fromUtf8(dataList[0]);
            QString computerName = QString::fromUtf8(dataList[1]);
            QString ipAddress = QString::fromUtf8(dataList[2]);
            QString userName = QString::fromUtf8(dataList[3]);

            qDebug() << "Domain:" << domain;
            qDebug() << "Computer Name:" << computerName;
            qDebug() << "IP Address:" << ipAddress;
            qDebug() << "User Name:" << userName;
        }

        clientSocket->disconnectFromHost();
    }
}
