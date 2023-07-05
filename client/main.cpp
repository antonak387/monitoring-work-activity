#include <QCoreApplication>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QTimer>

// Функция для создания клиента и установки соединения с сервером
void connectToServer()
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("localhost", 1234); // Укажите IP-адрес и порт сервера

    if (!socket->waitForConnected(5000))
    {
        qDebug() << "Failed to connect to server!";
        socket->deleteLater();
        return;
    }

    qDebug() << "Connected to server!";

    // Получение текущего времени
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");

    // Получение информации о домене, компьютере, IP-адресе и пользователе (как в предыдущем примере)
    QString domain = QHostInfo::localDomainName();
    QString computerName = QSysInfo::machineHostName();
    QString ipAddress;
    QList<QHostAddress> ipAddresses = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : ipAddresses)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost)
        {
            ipAddress = address.toString();
            break;
        }
    }
    QString userName = qgetenv("USER"); // Для Unix-подобных систем
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // Для Windows

    // Создание строки данных, включающей время, домен, компьютер, IP-адрес и пользователя
    QString data = timeString + "," + domain + "," + computerName + "," + ipAddress + "," + userName;

    // Отправка данных на сервер
    socket->write(data.toUtf8());
    socket->waitForBytesWritten(5000);

    // Получение ответа от сервера
    socket->waitForReadyRead(5000);
    QByteArray response = socket->readAll();
    qDebug() << "Server response:" << response;

    socket->disconnectFromHost();
    socket->deleteLater();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Установка таймера для повторного подключения к серверу
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &connectToServer);
    timer.start(5000); // Интервал повторного подключения (в миллисекундах)

    // Подключение к серверу при запуске клиента
    connectToServer();

    return a.exec();
}
