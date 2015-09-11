#include "tactics.h"

/**
 * @brief Tactics::Tactics
 */
Tactics::Tactics() :
    bluetooth(new BluetoothMaster),
    ball(new Ball),
    robot(new Robot),
    isObjectsLoaded(false),
    beforeY(0)
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
    beforeY = ball->getY() - ball->startPoint.y();

    if( abs(beforeY-ball->getY()) > 16 )    {
        bluetooth->transaction(portName,
                              SERIAL_DELAYTIME,
                              parsingPositionData(QPoint(ball->getX() - ball->startPoint.x(),
                                                         ball->getY() - ball->startPoint.y())));
        beforeY = ball->getY();
    }
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
    if( (robotSide == LEFT_SIDE && (ball->direction == NORTH_WEST
                                    || ball->direction == SOUTH_WEST))
        || (robotSide == LEFT_SIDE && (ball->direction == NORTH_WEST
                                    || ball->direction == SOUTH_WEST)) )   {
        defense();
    }
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

    isObjectsLoaded = true;
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
 * @brief Tactics::slotRobotSideChanged
 */
void Tactics::slotRobotSideChanged(int)
{
    this->robotSide = (RobotSide)robotSide;
}

/**
 * @brief Tactics::parsingPositionData
 * @return
 */
QString Tactics::parsingPositionData(QPoint position)
{
    QByteArray request;
    QString stringX = NULL;
    QString stringY = NULL;

    int x = position.x() / robot->unitX;
    int y = position.y() / robot->unitY;

    stringX.append( (x >= 100) ? stringX.number(x)
                               : (x >= 10 ) ? "0" + stringX.number(x)
                                            : "00" +stringX.number(x));

    stringY.append( (y >= 100) ? stringY.number(y)
                               : (y >= 10 ) ? "0" + stringY.number(y)
                                            : "00" +stringY.number(y));

    request.append("G");
    request.append(stringX);
    request.append(stringY);
    request.append("E");
    qDebug("%s", request.data());

    return request;
}
