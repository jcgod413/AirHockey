#ifndef TACTICS_H
#define TACTICS_H

#include <QWidget>
#include "ahr.h"
#include "robot.h"
#include "ball.h"
#include "bluetoothmaster.h"

class Tactics : public QWidget
{
    Q_OBJECT

public:
    Tactics();
    ~Tactics();

    void defense();
    void offense();
    void sendSerial(QByteArray);
    void sendSerial(Packet_Major, Packet_Minor);
    QString parsingPositionData(QPoint);
    void radioStateChanged(RadioState);
    void calcDirection();

private:
    Ball *ball;
    Robot *robot;
    BluetoothMaster *bluetooth;

    QString portName;
    bool isObjectsLoaded;
    int beforeY;
    RobotSide robotSide;
    Packet_Minor robotDirection;

public slots:
    void slotRenewObjects(Ball*, Robot*);
    void slotPortNameChanged(QString);
    void slotStartAction();
    void slotRobotSideChanged(int);
};

#endif // TACTICS_H
