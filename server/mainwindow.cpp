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
    }
}

void MainWindow::processData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        QByteArray data = clientSocket->readAll();
        qDebug() << "Received data from client:" << data;

        // Разделение данных
        QList<QByteArray> dataList = data.split(',');

        if (dataList.size() >= 6) // Убираем лишнее
        {
            QString id = QString::fromUtf8(dataList[0]);
            int nowClient = -1;

            // Поиск существующего клиента по ID
            for (int i = 0; i < clientTable.size(); i++) {
                if (clientTable[i][0] == id) {
                    nowClient = i;
                    break;
                }
            }

            if (nowClient >= 0) {
                // Обновляем параметры существующего клиента
                clientTable[nowClient][1] = QString::fromUtf8(dataList[1]);
                clientTable[nowClient][2] = QString::fromUtf8(dataList[2]);
                clientTable[nowClient][3] = QString::fromUtf8(dataList[3]);
                clientTable[nowClient][4] = QString::fromUtf8(dataList[4]);
                clientTable[nowClient][5] = QString::fromUtf8(dataList[5]);
            } else {
                // Создаем нового клиента и добавляем его в таблицу
                QStringList newClient;
                newClient << id << QString::fromUtf8(dataList[1]) << QString::fromUtf8(dataList[2])
                          << QString::fromUtf8(dataList[3]) << QString::fromUtf8(dataList[4])
                          << QString::fromUtf8(dataList[5]);
                clientTable.append(newClient);
                nowClient = clientTable.size() - 1;

                ui->tableWidget->setRowCount(clientTable.size());
            }

            // Обновление таблицы
            for (int i = 0; i < columns; i++) {
                QTableWidgetItem *item = ui->tableWidget->item(nowClient, i);
                if (!item) {
                    item = new QTableWidgetItem;
                    ui->tableWidget->setItem(nowClient, i, item);
                }
                item->setText(clientTable[nowClient][i]);
            }
        }

        clientSocket->disconnectFromHost();
    }
}



