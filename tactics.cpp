#include "tactics.h"

/**
 * @brief Tactics::Tactics
 */
Tactics::Tactics() :
    bluetooth(new BluetoothMaster),
    ball(new Ball),
    robot(new Robot)
{

}

/**
 * @brief Tactics::~Tactics
 */
Tactics::~Tactics()
{

}

/**
 * @brief Tactics::defense
 */
void Tactics::defense()
{
    bluetooth->transaction(portName,
                          SERIAL_DELAYTIME,
                          parsingPositionData(QPoint(ball->predictPoint.x(),
                                                     ball->predictPoint.y())));
}

/**
 * @brief Tactics::offense
 */
void Tactics::offense()
{

}

/**
 * @brief Tactics::slotStartAction
 */
void Tactics::slotStartAction()
{
    /* ball not found */
    if( ball->pos().x() == 0 && ball->pos().y() == 0)   {
        return;
    }

    defense();
}

/**
 * @brief Tactics::slotRenewObjects
 * @param ball
 * @param robot
 */
void Tactics::slotRenewObjects(Ball *ball, Robot *robot)
{
    delete this->ball;
    delete this->robot;

    this->ball = ball;
    this->robot = robot;
}

/**
 * @brief Tactics::slotPortNameChanged
 * @param portName
 */
void Tactics::slotPortNameChanged(QString portName)
{
    this->portName = portName;
}

/**
 * @brief Tactics::parsingPositionData
 * @return
 */
QString Tactics::parsingPositionData(QPoint position)
{
    int x = position.x();
    int y = position.y();
    QString stringX = NULL;
    QString stringY = NULL;

    while( x > 10 ) {
        stringX.append("0");
        x/=10;
    }
    stringX.append(stringX.number(x));

    while( y > 10 ) {
        stringY.append("0");
        y/=10;
    }
    stringY.append(stringY.number(y));

    return 'G' + stringX + stringY + 'E';
}
