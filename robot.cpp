#include "robot.h"

/**
 * @brief Robot::Robot
 */
Robot::Robot() :
    position(0, 0)
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
