#ifndef BLUETOOTHMASTER_H
#define BLUETOOTHMASTER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QTime>

class BluetoothMaster : public QThread
{
    Q_OBJECT
public:
    BluetoothMaster(QObject *parent = 0);
    ~BluetoothMaster();

    void transaction(const QString &portName, int waitTimeout, const QString &request);
    void run();

private:
    QString portName;
    QString request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;

signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);
};

#endif // BLUETOOTHMASTER_H
