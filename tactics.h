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
    QString parsingPositionData(QPoint);

private:
    Ball *ball;
    Robot *robot;
    BluetoothMaster *bluetooth;

    QString portName;
    bool isObjectsLoaded;
    int beforeY;

public slots:
    void slotRenewObjects(Ball*, Robot*);
    void slotPortNameChanged(QString);
    void slotStartAction();
};

#endif // TACTICS_H
