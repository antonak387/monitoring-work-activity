#include <QCoreApplication>
#include <QTcpSocket>
#include <QDebug>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTcpSocket socket;
    socket.connectToHost("localhost", 1234); // Укажите IP-адрес и порт сервера

    if (!socket.waitForConnected(5000))
    {
        qDebug() << "Failed to connect to server!";
        return -1;
    }

    qDebug() << "Connected to server!";

    // Получение информации о домене
    QString domain = QHostInfo::localDomainName();
    qDebug() << "Domain:" << domain;

    // Получение информации о компьютере
    QString computerName = QSysInfo::machineHostName();
    qDebug() << "Computer Name:" << computerName;

    // Получение информации об IP-адресе
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
    qDebug() << "IP Address:" << ipAddress;

    // Получение информации о пользователе
    QString userName = qgetenv("USER"); // Для Unix-подобных систем
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // Для Windows
    qDebug() << "User Name:" << userName;

    // Отправка данных на сервер
    QString data = domain + "," + computerName + "," + ipAddress + "," + userName;
    socket.write(data.toUtf8());
    socket.waitForBytesWritten(5000);

    // Получение ответа от сервера
    socket.waitForReadyRead(5000);
    QByteArray response = socket.readAll();
    qDebug() << "Server response:" << response;

    socket.disconnectFromHost();

    return a.exec();
}
