#ifndef TACTICS_H
#define TACTICS_H

#include <QWidget>
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

public slots:
    void slotRenewObjects(Ball*, Robot*);
};

#endif // TACTICS_H
