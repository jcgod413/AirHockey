#include "bluetoothmaster.h"

/**
 * @brief BluetoothMaster::BluetoothMaster
 * @param parent
 */
BluetoothMaster::BluetoothMaster(QObject *parent) :
    QThread(parent),
    waitTimeout(0),
    quit(false)
{

}

/**
 * @brief BluetoothMaster::~BluetoothMaster
 */
BluetoothMaster::~BluetoothMaster()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

/**
 * @brief BluetoothMaster::transaction
 * @param portName
 * @param waitTimeout
 * @param request
 */
void BluetoothMaster::transaction(const QString &portName,
                                  int waitTimeout,
                                  const QByteArray &request)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->request = request;

    qDebug("%s", request.data());

    if(!isRunning())
    {
        start();
    }
    else
        cond.wakeOne();
}

/**
 * @brief BluetoothMaster::run
 */
void BluetoothMaster::run()
{
    bool currentPortNameChanged = false;

    mutex.lock();

    QString currentPortName;
    if(currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QByteArray currentRequest = request;
    mutex.unlock();

    QSerialPort serial;

    while (!quit) {

        if(currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite))  {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }

        QByteArray requestData = currentRequest;
        serial.write(requestData);
        if (serial.waitForBytesWritten(waitTimeout))    {

            if(serial.waitForReadyRead(currentWaitTimeout)) {
                QByteArray responseData = serial.readAll();
                while(serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                QString response(responseData);

                emit this-> response(response);

            } else {
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }

        } else {
            emit timeout(tr("Wait write request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }

        mutex.lock();
        cond.wait(&mutex);
        if(currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged =true;
        }   else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
