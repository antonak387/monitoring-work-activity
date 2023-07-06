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
        return;
    }
    ui->tableWidget->hideColumn(0);
    ui->tableWidget->setColumnWidth(6, 384);


}

MainWindow::~MainWindow()// деструктор
{
    delete ui;
    server->close();
    clients.clear();
    for (int i = 0; i < clientTable.size(); ++i)
    {
        clientTable[i].clear();  // Очистить QStringList
    }
    clientTable.clear();

}

void MainWindow::newConnection()//новое соединение
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *clientSocket = server->nextPendingConnection();
        clients.append(clientSocket);

        connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::processData);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

void MainWindow::processData()
{

    QImage receivedImage;

    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        QByteArray data = clientSocket->readAll();

        // Разделение данных
        QList<QByteArray> dataList = data.split(',');

        QByteArray pngMarker = "\x89PNG";
        int pngIndex = data.indexOf(pngMarker);
        QByteArray pngData;
        if (pngIndex != -1) {
            pngData = data.mid(pngIndex);
        }
        if (dataList.size() >= 7) // Убираем лишнее
        {

            QByteArray imageData = pngData;
            receivedImage.loadFromData(imageData);

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

                // Загрузка принятого изображения


                // Обновление таблицы, включая изображение
                for (int i = 0; i < columns; i++) {
                    QTableWidgetItem *item = ui->tableWidget->item(nowClient, i);
                    if (!item) {
                        item = new QTableWidgetItem;
                        ui->tableWidget->setItem(nowClient, i, item);
                    }

                    if (i == 6) {
                        // Седьмой столбец - изображение
                        item->setData(Qt::DecorationRole, QPixmap::fromImage(receivedImage));

                    } else {
                        item->setText(clientTable[nowClient][i]);
                    }
                }
            } else {
                // Создаем нового клиента и добавляем его в таблицу
                QStringList newClient;
                newClient << id << QString::fromUtf8(dataList[1]) << QString::fromUtf8(dataList[2])
                          << QString::fromUtf8(dataList[3]) << QString::fromUtf8(dataList[4])
                          << QString::fromUtf8(dataList[5]);
                clientTable.append(newClient);

                nowClient = clientTable.size() - 1;

                ui->tableWidget->setRowCount(clientTable.size());
                ui->tableWidget->setRowHeight(nowClient, 216);

                for (int i = 0; i < columns; i++) {
                    QTableWidgetItem *item = ui->tableWidget->item(nowClient, i);
                    if (!item) {
                        item = new QTableWidgetItem;
                        ui->tableWidget->setItem(nowClient, i, item);
                    }

                    if (i == 6) {
                        // Седьмой столбец - изображение
                        item->setData(Qt::DecorationRole, QPixmap::fromImage(receivedImage));

                    } else {
                        item->setText(clientTable[nowClient][i]);
                    }
                }

            }
        }

        clientSocket->disconnectFromHost();
    }
}
