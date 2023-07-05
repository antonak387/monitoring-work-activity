#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QPixmap>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newConnection();
    void processData();

private:
    Ui::MainWindow *ui;

    QTcpServer *server;
    QList<QTcpSocket*> clients;

    QString* clientId;
    int numClient = 0;

    QVector<QStringList> clientTable;
    const int columns = 7;

};

#endif // MAINWINDOW_H
