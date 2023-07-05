#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

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
    const int columns = 6;


};

#endif // MAINWINDOW_H
