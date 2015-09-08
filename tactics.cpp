#include "tactics.h"

/**
 * @brief Tactics::Tactics
 */
Tactics::Tactics() :
    bluetooth(new BluetoothMaster)
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
    bluetooth.transaction(ui->comboBox->currentText(),
                          ui->spinBox->value(),
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
 * @brief Tactics::slotRenewObjects
 * @param ball
 * @param robot
 */
void Tactics::slotRenewObjects(Ball *ball, Robot *robot)
{
    this->ball = ball;
    this->robot = robot;
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
