#include "robot.h"

/**
 * @brief Robot::Robot
 */
Robot::Robot() :
    position(0, 0),
    redMax(0),
    greenMax(0),
    blueMax(0),
    redMin(255),
    greenMin(255),
    blueMin(255)
{

}

/**
 * @brief Robot::~Robot
 */
Robot::~Robot()
{

}

/**
 * @brief Robot::slotRenewPosition
 * @param position
 */
void Robot::slotRenewPosition(QPoint position)
{
    this->position = position;
}

/**
 * @brief Robot::updatePosition
 * @param position
 */
void Robot::updatePosition(QPoint position)
{
    this->position = position;
}

/**
 * @brief Robot::updateColor
 * @param red
 * @param green
 * @param blue
 */
void Robot::updateColor(int red, int green, int blue)
{
    redMax   = (redMax<red) ? red : redMax;
    redMin   = (redMin>red) ? red : redMin;
    greenMax = (greenMax<green) ? green : greenMax;
    greenMin = (greenMin>green) ? green : greenMin;
    blueMax  = (blueMax<blue) ? blue : blueMax;
    blueMin  = (blueMin>blue) ? blue : blueMin;

    maxColor = QColor(redMax, greenMax, blueMax);
    minColor = QColor(redMin, greenMin, blueMin);
}

/**
 * @brief Robot::resetColor
 */
void Robot::resetColor()
{
    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;

    maxColor = QColor(redMax, greenMax, blueMax);
    minColor = QColor(redMin, greenMin, blueMin);
}

/**
 * @brief Robot::getMaxColor
 * @return
 */
QColor Robot::getMaxColor()
{
    return maxColor;
}

/**
 * @brief Robot::getMinColor
 * @return
 */
QColor Robot::getMinColor()
{
    return minColor;
}

/**
 * @brief Robot::getPosition
 * @return
 */
QPoint Robot::getPosition()
{
    return position;
}
